#include "FRT.h"

#define QUEUE_SIZE 100
#define P_NAME_LEN 23
#define F_NAME_LEN 21
#define A_NAME_LEN 26
#define C_NAME_LEN 23

QueueHandle_t xQueue;
SemaphoreHandle_t xPrintMutex = NULL;


/*
 *Indicates the amount of department cars are currently executing a task
*/
uint8_t PoliceCarsOnDuty = 0;
uint8_t FireCarsOnDuty = 0;
uint8_t AmbulanceCarsOnDuty = 0;
uint8_t CoronaCarsOnDuty = 0;

/*
 *Initial seed value, part of the function 'generateRandomNumber()' generated from CHATGPT
*/
static uint32_t prng_seed = 12345;

void main_FRT(){
	/*
	 *Helps to identify the beginning of a new process in the console.
	*/
	PaddingStartOfFunc();


	printf("Start of the program\r\n");

	/*
	 *Initializing stage 'Message Queue' and 'Mutex'
	*/
	uint8_t Department = 0;
	xQueue = xQueueCreate( QUEUE_SIZE, sizeof(Department));
	xPrintMutex = xSemaphoreCreateMutex();

	/*
	 *If there is an error in the initializing stage, then the code won't run and the main task will enter an infinity loop
	*/
	if((xQueue != NULL) && (xPrintMutex != NULL)){
		BaseType_t err = 0;

		/*
		 *Creating the Dispatcher task that will run in the background. His role is to send message using a queue that will be read by the department tasks
		*/
		err = xTaskCreate(DispatcherTask, "DispatcherTask", LARGE_TASK_SIZE, NULL, ABOVE_NORMAL, NULL);
		if(err != pdPASS){
			printf("Error xTaskCreate DispatcherTask\r\n");
			exit(1);
		}

		/*
		 *Creating police department tasks. Each task represents a police car.
		*/
		for(uint8_t i = 0; i < MAX_POLICE_CARS; i++){
			char TaskName[P_NAME_LEN];
			sprintf(TaskName, "PoliceDepartmentTask %d", i + 1);
			err = xTaskCreate(PoliceDepartmentTask, &TaskName, SMALL_TASK_SIZE, NULL, ABOVE_NORMAL , NULL);
				if(err != pdPASS){
				printf("Error xTaskCreate PoliceDepartmentTask\r\n");
				exit(1);
				}
		}

		/*
		 *Creating fire department tasks. Each task represents a fire car.
		*/

		for(uint8_t i = 0; i < MAX_FIRE_CARS; i++){
			char TaskName[F_NAME_LEN];
			sprintf(TaskName, "FireDepartmentTask %d", i + 1);
			err = xTaskCreate(FireDepartmentTask, TaskName, SMALL_TASK_SIZE, NULL, ABOVE_NORMAL, NULL);
			if(err != pdPASS){
				printf("Error xTaskCreate FireDepartmentTask\r\n");
				exit(1);
			}
		}


		/*
		 *Creating ambulance department tasks. Each task represents a ambulance car.
		*/

		for(uint8_t i = 0; i < MAX_AMBULANCE_CARS; i++){
			char TaskName[A_NAME_LEN];
			sprintf(TaskName, "AmbulanceDepartmentTask %d", i + 1);
			err = xTaskCreate(AmbulanceDepartmentTask, TaskName, LARGE_TASK_SIZE, NULL, ABOVE_NORMAL, NULL);
			if(err != pdPASS){
				printf("Error xTaskCreate AmbulanceDepartmentTask\r\n");
				exit(1);
			}
		}


		/*
		 *Creating corona department tasks. Each task represents a corona car.
		*/

		for(uint8_t i = 0; i < MAX_CORONA_CARS; i++){
			char TaskName[C_NAME_LEN];
			sprintf(TaskName, "CoronaDepartmentTask %d", i + 1);
			err = xTaskCreate(CoronaDepartmentTask, TaskName, LARGE_TASK_SIZE, NULL, ABOVE_NORMAL, NULL);
			if(err != pdPASS){
				printf("Error xTaskCreate CoronaDepartmentTask\r\n");
				exit(1);
			}
		}
		vTaskStartScheduler();
	}
	else{
		printf("Error in Initializing 'Message Queue' and 'Mutex'\r\n");
	}

	/*
	 *If there was a failure in initializing stage of 'Message Queue' and 'Mutex', then the main tasks enters an infinity loop
	*/

	for(;;);

	/*
	 *Will never get here
	*/
	return;
}

void DispatcherTask(void const * argument)
{
	/*
	 *Initializing the department variable which indicates to whom the message is addressed
	*/
	uint32_t Department = 0;
	BaseType_t err = 0;

	for(;;)
	{
		/*
		 *'Randomly' generating a number that represents to which department the message will be sent.
		 *Then sends it to the chosen department
		*/
		Department = (generateRandomNumber(0,100) % 4) + 1;
		err = xQueueSend(xQueue, (void*) &Department, portMAX_DELAY);
		if(err != pdTRUE){
			printf("Error xQueueSend\r\n");
			vTaskDelete(NULL);
		}
	}
	vTaskDelete(NULL);
}


void PoliceDepartmentTask(void const * argument)
{
	/*
	 *Variables that a value will be stored in them later
	*/
	uint8_t Department = 0;
	uint32_t TaskTimeIteration = 0;

	for(;;)
	{
		/*
		 *The task peeks at the top message to see if it's addressed to it
		*/
		if (xQueuePeek(xQueue, &Department, portMAX_DELAY)) {
			/*
			 *If the message is indeed addressed to it, the task reads it and clears the top of the queue.
			 *Thus making room for a new message
			*/
			if(Department == 1){
				xQueueReceive(xQueue,(void*) &Department, portMAX_DELAY);

				/*
				 *Taking a mutex in order to write to the console
				*/
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				/*
				 *Updating the number of cars of the department which have a task
				*/
				PoliceCarsOnDuty++;
				printf("Police cars on duty: %d\r\n",PoliceCarsOnDuty);
				xSemaphoreGive(xPrintMutex);

				/*
				 *Doing the task
				*/
				for(TaskTimeIteration = 0; TaskTimeIteration < generateRandomNumber(MIN, MAX); TaskTimeIteration++);

				/*
				 *Updating the number of cars doing a task, after finishing their own task.
				*/
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				PoliceCarsOnDuty--;
				printf("Police cars on duty: %d\r\n",PoliceCarsOnDuty);
				xSemaphoreGive(xPrintMutex);
			}
		}
	}
	vTaskDelete(NULL);
}

void AmbulanceDepartmentTask(void const * argument)
{
	uint8_t Department = 0;
	uint32_t TaskTimeIteration = 0;

	for(;;)
	{
		/*
		 *The task peeks at the top message to see if it's addressed to it
		*/
		if (xQueuePeek(xQueue,(void*) &Department, portMAX_DELAY)) {

			/*
			 *If the message is indeed addressed to it, the task reads it and clears the top of the queue.
			 *Thus making room for a new message
			*/
			if(Department == 2){
				xQueueReceive(xQueue, &Department, portMAX_DELAY);

				/*
				 *Taking a mutex in order to write to the console
				*/
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				/*
				 *Updating the number of cars of the department which have a task
				*/
				AmbulanceCarsOnDuty++;
				printf("Ambulance cars on duty: %d\r\n",AmbulanceCarsOnDuty);
				xSemaphoreGive(xPrintMutex);

				/*
				 *Doing the task
				*/
				for( TaskTimeIteration = 0; TaskTimeIteration < generateRandomNumber(MIN, MAX); TaskTimeIteration++);

				/*
				 *Updating the number of cars doing a task, after finishing their own task.
				*/
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				AmbulanceCarsOnDuty--;
				printf("Ambulance cars on duty: %d\r\n",AmbulanceCarsOnDuty);
				xSemaphoreGive(xPrintMutex);
			}
		}
	}
	vTaskDelete(NULL);
}


void FireDepartmentTask(void const * argument)
{
	uint8_t Department = 0;
	uint32_t TaskTimeIteration = 0;
	for(;;)
	{
		/*
		 *The task peeks at the top message to see if it's addressed to it
		*/
		if (xQueuePeek(xQueue, &Department, portMAX_DELAY)) {

			/*
			 *If the message is indeed addressed to it, the task reads it and clears the top of the queue.
			 *Thus making room for a new message
			*/
			if(Department == 3){
				xQueueReceive(xQueue,(void*) &Department, portMAX_DELAY);

				/*
				 *Taking a mutex in order to write to the console
				*/
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				/*
				 *Updating the number of cars of the department which have a task
				*/
				FireCarsOnDuty++;
				printf("Fire cars on duty: %d\r\n",FireCarsOnDuty);
				xSemaphoreGive(xPrintMutex);

				/*
				 *Doing the task
				*/
				for( TaskTimeIteration = 0; TaskTimeIteration < generateRandomNumber(MIN, MAX); TaskTimeIteration++);

				/*
				 *Updating the number of cars doing a task, after finishing their own task.
				*/
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				FireCarsOnDuty--;
				printf("Fire cars on duty: %d\r\n",FireCarsOnDuty);
				xSemaphoreGive(xPrintMutex);
			}
		}
	}
	vTaskDelete(NULL);
}

void CoronaDepartmentTask(void const * argument)
{
	uint8_t Department = 0;
	uint32_t TaskTimeIteration = 0;

	for(;;)
	{
		/*
		 *The task peeks at the top message to see if it's addressed to it
		*/
		if (xQueuePeek(xQueue, &Department, portMAX_DELAY)) {

			/*
			 *If the message is indeed addressed to it, the task reads it and clears the top of the queue.
			 *Thus making room for a new message
			*/
			if(Department == 4){

				/*
				 *Taking a mutex in order to write to the console
				*/
				xQueueReceive(xQueue,(void*) &Department, portMAX_DELAY);
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				/*
				 *Updating the number of cars of the department which have a task
				*/
				CoronaCarsOnDuty++;
				printf("Corona cars on duty: %d\r\n",CoronaCarsOnDuty);
				xSemaphoreGive(xPrintMutex);

				/*
				 *Doing the task
				*/
				for( TaskTimeIteration = 0; TaskTimeIteration < generateRandomNumber(MIN, MAX); TaskTimeIteration++);

				/*
				 *Updating the number of cars doing a task, after finishing their own task.
				*/
				xSemaphoreTake(xPrintMutex, portMAX_DELAY);
				CoronaCarsOnDuty--;
				printf("Corona cars on duty: %d\r\n",CoronaCarsOnDuty);
				xSemaphoreGive(xPrintMutex);
			}
		}
	}
	vTaskDelete(NULL);
}

// Basic linear congruential generator (LCG) PRNG algorithm,part of the function 'generateRandomNumber()' Created by CHATGPT, to save time
uint32_t randNum(void) {
    prng_seed = (prng_seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return prng_seed;
}

// Function to generate a random number within a specified range [min, max] Created by CHATGPT to save time
uint32_t generateRandomNumber(uint32_t min, uint32_t max) {
    // Ensure the range is valid
    if (min > max) {
        // Swap min and max if needed
        uint32_t temp = min;
        min = max;
        max = temp;
    }

    // Calculate the random number within the specified range
    return (rand() % (max - min + 1)) + min;
}

void PaddingStartOfFunc(void){
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");
	printf("\r\n");
}

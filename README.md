# FreeRT_Dispatcher
# The code was generated for NUCLEO-F746ZG board

In this project I had to simulate a dispathcer unit using a 'Message Queue' IPC. There job is to navigate incoming calls between 4 departments: Police, Fire, Ambulance and Corona.
Each department has its maximum number of cars, each car is a different thread, that read a task only if it addressed to their deparment and the message is read right from the queue.
Before starting the task the car thread removes the message from the queue. A department cant send more cars than it have.
If there are no task to do or not enough of them to send all the departmnets car, then the car that has no task is in blocking state until a new task appeares in the queue.
When a car goes to solve a task or if it has finished it, the department will send a message to the console with the cuerrent task that are being solved. 
Each depertments max number of cars:
1) Police - 3
2) Fire - 2
3) Ambulance - 4
4) Corona - 4

The code has to have Multithreading and IPCs while runnig the FreeRTOS O.S.
I used the next IPCs: Message Queue and Mutex.

The code generates a lot of masseages that indecates the amount of tasks each department has.
Since in real life a call to the dispatcher unit can come in intervals of minuts, I wrote it as if it comes with a varying intervals, from milliseconds to seconds, so it is easier to run the code.

## Introduction 

The following source code is a solution for a simmulated Deadline Driven Scheduler using FreeRTOS. This solution
is an embedded software capable of creating and scheduling real time tasks, assigning priority
based on their deadline for completion as shown in Figure 1 below:

![image](https://user-images.githubusercontent.com/61804317/175792993-eb665622-40f1-44e9-879f-7c187c4b5d2f.png)

This project was developed as an embedded software solution onboard an STM32F0 discovery
board, utilizing the open source STM32F4 and FreeRTOS API’s. The final solution consisted of
the following FreeRTOS objects: 2 xTasks, 2 queues but required several helper functions
to accommodate the linked list manipulation required to create the tasks.
 

## Design Solution 

Below shows the system overview diagram for the proposed solution: 

![image](https://user-images.githubusercontent.com/61804317/175793033-c13cc317-75bb-4bfa-bb9f-48b1435f3193.png)


As shown in taskList.h, each task is defined as a pointer to a node that can
be inserted into a doubly-linked list as shown below:

![image](https://user-images.githubusercontent.com/61804317/175793106-d6db927b-d2d2-447c-8821-2262e51c0879.png)

The above instrinsic parameters are fairly self explanatory to the DD_Task structure. The only
notable change from the lab manual is the task state. The task state is used to determine which 
9
state (Active, Released, Created, Completed, Overdue) the task is in. The task is in “created sate”
upon creation then once the DDS has determined it is to be realeased, it will toggle the ACTIVE
state allowing it to complete its routine. See code snippet from UserdefinedTasks.c below to
Active State is blocking the tasks execution: 

![image](https://user-images.githubusercontent.com/61804317/175793109-5ab9c5ff-3113-477c-9d39-c095ce4e4199.png)

Doubly-linked lists were chosen as the dataset to manage the task lists since it offers optimal
control over list sorting if an item were to be placed in the middle of the linked list. 

![image](https://user-images.githubusercontent.com/61804317/175793117-c48f0b06-0906-461b-9fc4-5178c6738286.png)

Below: Shows a code snippet for how the tasks are generated: 

![image](https://user-images.githubusercontent.com/61804317/175793122-b91d3a55-8a1a-493a-9884-c4e7e3d746eb.png)

As shown in taskList.h, each task list (ACTIVE, OVERDUE, COMPLETD) is defined as doubly
linked list keeping track of the state of each task node: 

### DDT Sorting Flow Chart

![image](https://user-images.githubusercontent.com/61804317/175793129-e6da8b60-4b59-467c-9e56-32aef34f05d6.png)

Below shows the flow chart for the main DDS function.
The DDS essentially receives a message from each of the DDS_Function. Will subjugate the
processing of the task through changing its intrinsic parameter “Task_State” then based on the
deadline of the task, the taks will be sorted and then finally inserted into one of the 3 doubly
linked lists awaiting execution. 


![image](https://user-images.githubusercontent.com/61804317/175793141-2604a430-2857-4ac3-a5d6-eb704a406e70.png)

### Results of Tast Benches 

As shown below, the designed DDS scheduler starts at 3 ticks which makes up for the difference
in the expected outputs. 

![image](https://user-images.githubusercontent.com/61804317/175793150-0dc5a580-f046-4d3d-9958-b97cdab46700.png)

![image](https://user-images.githubusercontent.com/61804317/175793159-73968bba-ef5a-4125-a946-63554bf2a03f.png)


As shown in the comparison table below, the DDS was tracking task states as expected. 

![image](https://user-images.githubusercontent.com/61804317/175793166-e8de4191-a0a9-4725-83ac-0a6c127f9281.png)

![image](https://user-images.githubusercontent.com/61804317/175793171-b647cf11-8924-4f17-8218-9908c850f746.png)

## Limitaitons and Possible Improvments 

Some possible limitations of this software design include the following: 

Most of the limitations of this design are as a result of the decision to use 3 doubly linked lists to
sort the tasks into the active, overdue and completed statues



*1. Limitations in scalability*
  - undamentally, using doubly linked lists has significant overhead in keeping track
    of list items. As mentioned above, doubly linked lists were chosen since it offers
    the most flexibility to insert and sort nodes, however, the overhead required to
    sort the lists makes this solution far more complicated to trouble shoot and it took
    significant time to trouble shoot when running into memory initialization
    problems. The code is set up to support more tasks than 3, however there is no
    doubt that more tasks will vastly diminish the performance of this architecture.

*2. Malloc Failures:*
  - The way this program is structured with 3 doubly linked lists and sorting tasks
    between them eventually causes a malloc failure after several hyper periods. This
    is a result of the nodes being cast between the various lists and the juggling of
    memory within the monitor task pre-emption.

*3. Software delay to pre-empt monitor task:*
  - As shown in the code snipped below, a software delay was used to periodically
    pre-empt the current task being executed with the monitor task. This was chosen
    for simplicity but is not a very robust solution since were are solely relying on an
    API function to update the monitor. This was evident when stress testing this code
    as some contention was found as a result of the monitor task executing without
    the current task being put into the block state. 

    ![image](https://user-images.githubusercontent.com/61804317/175793214-6f26f680-416b-4d30-9e1c-e85f97d30fdc.png)

## Possible Improvments 

 **1. Replacing the double linked lists API with an equivalent singly linked list API** 
  - Replacing the entire API will make it much harder to sort tasks in the event that a
    middle of lists, however this complexity in programming is ultimately much more
    scalable to accommodate more tasks and juggling the changing task states.
    
  **2. Malloc Failures:** 
    - A robust singly linked list backend architecture will make tracking memory
      allocation much more efficient. Additionally, the create_dd_task function could
      be improved to mitigate the potential for malloc failures as the current design
      involves using the task_exists_in_List() function to return a pointer to the
      memory location within the list:
 
   ![image](https://user-images.githubusercontent.com/61804317/175793278-9d9711d5-1b76-4bcd-ab5c-ff5fda2999d3.png)


  **3. Changing the delay driven Monitor_Task to an event driven Monitor_Task**

   - Having an event trigger the task-preemption will ensure that the current task is in
     a block state preventing the contention experienced when stress testing the
     program. 


## Summary 

The proposed solution outlined in this report was successfully. The final solution consisted of 2 F-task (DDS,
Monitor_Task), the 4 DDS fucntions communication (delete_dd_Task(),
release_dd_Task(),complete_dd_Task(),create_dd_Task()), 1 software timers ( to control each
execution time and periotic release) and several helper functions to support the task sorting and
prioritization between the DDS functions, subjugated by the DDS. The software infrastructure
for the final solution provided a lot of flexibility and modularity as required, still some
improvements to its architecture could be made to both scale and fully optimized its
performance. 



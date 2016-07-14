#include "ClassDeclarations.h"

//No need for closing, since we can just query if LockedSPSCQueue is invalid inside the lock
	//Need to think over this a bit

//Here is a version of the lockless queue using mutexes
//Here the slow down comes from the face that there is a single mutex
	//Since it is supposed to single producer single consumer each thread should only be operating on seperate sides of the function
	//under most cases the two threads should almost never intersect

LockedSPSCQueue::LockedSPSCQueue(int data)
{
	LockedNode* temp = (LockedNode*)malloc(sizeof(LockedNode));
	temp->data = data;
	temp->next = nullptr;

	head = tail = temp;	//make sure this is ordered correctly, probably is
}
LockedSPSCQueue::~LockedSPSCQueue()
{
	//I locked the destructor, so that a thread may destroy the queue and something won't try to enqueue or anything like that while it is being destroyed
	mtx.lock();
	//Go through and destroy every node
	for (int i = 0; i < numNodes; i++)
	{
		LockedNode* temp = head;
		head = head->next;
		free(temp);
	}
	mtx.unlock();
}

bool LockedSPSCQueue::enqueue(int data)
{
	//make the new node
	LockedNode* newNode = (LockedNode*)malloc(sizeof(LockedNode));
	newNode->data = data;
	newNode->next = nullptr;

	//Lock the critical section
	mtx.lock();
	//check if there is nothing in the queue and if so, set head equal to the newNode, else set tail->next to newNode
	if (tail == nullptr)
		head = newNode;
	else
		tail->next = newNode;

	//update the tail node to the newNode
	tail = newNode;
	//increase the size of the queue
	++numNodes;
	mtx.unlock();
	//unlock, because you are done

	return true;
}

int LockedSPSCQueue::dequeue()
{
	mtx.lock();
	//Get a temporary version of the head node's pointer, so we can free it outside the mutex
		//Looked like it was multiple assembly instructions
		/*008399D4  mov eax,dword ptr [this]  
		  008399D7  mov ecx,dword ptr [eax+8]  */
	LockedNode* temp = head;

	//Check if there is nothing in the queue and if so return NULL
	if (head == nullptr)
		return NULL;

	//Check if there is only one item in the queue and if so, set tail to a nullptr
		//won't work for the case of head == tail == nullptr, since of the above if statement
	if (head == tail)
		tail = nullptr;

	//set head to head's next node
		//if head is the only node in the queue, then head->next will be a nullptr and so head will be set to nullptr
	head = head->next;

	//decrement the number of nodes
	--numNodes;
	mtx.unlock();

	//save the data in a temporary variable
	int data = temp->data;
	//free the old head node
	free(temp);
	//return the data
	return data;
}

//An encapsulation method that just returns the size
int LockedSPSCQueue::size()
{
	return numNodes;
}
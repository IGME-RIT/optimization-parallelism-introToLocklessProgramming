#include "ClassDeclarations.h"

//No need for closing, since we can just query if LockedSPSCQueue is invalid inside the lock
	//Need to think over this a bit

LockedSPSCQueue::LockedSPSCQueue(int data)
{
	LockedNode* temp = (LockedNode*)malloc(sizeof(LockedNode));
	temp->data = data;
	temp->next = nullptr;

	head = tail = temp;	//make sure this is ordered correctly, probably is
}
LockedSPSCQueue::~LockedSPSCQueue()	//can see issues with this, where one thread deletes the queue and the other thread tries to use it	
										//double check locking? //do I actually care
{
	for (int i = 0; i < numNodes; i++)
	{
		LockedNode* temp = head;
		head = head->next;
		free(temp);
	}
}

bool LockedSPSCQueue::enqueue(int data)
{
	LockedNode* newNode = (LockedNode*)malloc(sizeof(LockedNode));	//malloc, might cause issues(different threads saving and freeing memory in the heap)
	newNode->data = data;
	newNode->next = nullptr;

	mtx.lock();
	tail->next = newNode;
	tail = newNode;
	++numNodes;
	mtx.unlock();

	return true;
}

int LockedSPSCQueue::dequeue()
{
	LockedNode* temp;
	temp = head;	//Just make sure this is okay

	mtx.lock();
	head = head->next;
	--numNodes;
	mtx.unlock();

	int data = temp->data;
	free(temp);
	return data;
}

int LockedSPSCQueue::size()
{
	return numNodes;
}
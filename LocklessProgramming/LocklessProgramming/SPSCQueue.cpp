#include "ClassDeclarations.h"

//SO first lets talk about some design decisions
//first we will go with a stereotypical FIFO ordering, just because ...
//second since this is a Single Producer and Single Consumer Queue,
	//the biggest things we have to watch out for are when a enqueue and dequeue are happenening simutaneouly and there are 2 < nodes

//Need

SPSCQueue::SPSCQueue()
{
	head = nullptr;
	tail = nullptr;
	numNodes = 0;
}

SPSCQueue::~SPSCQueue()
{
	int temp = numNodes.load(std::memory_order_acquire);
	for (int i = 0; i < temp; i++)
	{
		printf("Num: %d\n", dequeue());
	}
	
}


bool SPSCQueue::enqueue(int data)
{
	if (numNodes == QUEUE_MAX)
		return false;

	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = nullptr;
	newNode->prev = tail;
	


	//the three below operations must be atoics
	//Need to think over their memory ordering
	
	if (numNodes.load(std::memory_order_acquire) == 0)
	{
		head = newNode;
	}
	else
	{
		tail->next = newNode;
	}
	tail = newNode;

	numNodes.fetch_add(1, std::memory_order_acq_rel);

	return true;
}

int SPSCQueue::dequeue()
{
	Node* temp = head;
	head = head->next;
	if (numNodes > 1)
		head->prev = nullptr;

	//The above all need to be atomic and need to go off first

	int tempData = temp->data;
	numNodes--;
	free(temp);
	return tempData;
}

uint32_t SPSCQueue::size()
{
	return numNodes;
}
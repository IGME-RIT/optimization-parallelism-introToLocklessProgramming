#include "ClassDeclarations.h"

//SO first lets talk about some design decisions
//first we will go with a stereotypical FIFO ordering, just because ...
//second since this is a Single Producer and Single Consumer Queue,
	//the biggest things we have to watch out for are when a enqueue and dequeue are happenening simutaneouly and there are 2 < nodes

//Need

//A version of the constructor that initializes the list with a single node
SPSCQueue::SPSCQueue(int data)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = nullptr;
	head = newNode;
	tail = newNode;
	closing = false;
	numNodes = 1;
}

SPSCQueue::~SPSCQueue()
{
	/*bool tempClosing = false;
	if (!closing.compare_exchange_strong(tempClosing, true, std::memory_order_acq_rel, std::memory_order_relaxed))
		return;*/

	closing.store(true, std::memory_order_release);	//probably going to have issues, but might not
	int temp = numNodes.load(std::memory_order_acquire);
	for (int i = 0; i < temp; i++)
	{
		printf("Num: %d\n", dequeue());
	}

	Node* tempTail2 = back();
	
}


Node* SPSCQueue::front()
{
	return head;
}

Node* SPSCQueue::back()
{
	return tail;
}

//Very heavily used: http://kukuruku.co/hub/cpp/lock-free-data-structures-introduction
	//Will make some modifications

//Also:
	//https://www.research.ibm.com/people/m/michael/podc-1996.pdf

	//So far,
		//want to looks at the while loops and maybe make some changes
		//does not deal with the case where dequeue can be called without anything in the queue

bool SPSCQueue::enqueue(int data)
{
	if (numNodes.load(std::memory_order_acquire) == QUEUE_MAX)
		return false;

	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = nullptr;
	
	Node* tempTail;

	//might want to make some changes to the loop
	while (true)
	{
		tempTail = tail.load(std::memory_order_acquire);
		Node* tempNext = tempTail->next.load(std::memory_order_acquire);
		if (tempNext != nullptr)
		{
			tail.compare_exchange_weak(tempTail, tempNext, std::memory_order_release, std::memory_order_relaxed);
		}

		Node* temp = nullptr;
		if (tempTail->next.compare_exchange_strong(temp, newNode, std::memory_order_release, std::memory_order_relaxed))
			break;
	}

	tail.compare_exchange_strong(tempTail, newNode, std::memory_order_acq_rel, std::memory_order_relaxed);
	numNodes.fetch_add(1, std::memory_order_acq_rel);
	return true;
}

int SPSCQueue::dequeue()
{
	Node* tempHead = nullptr;
	Node* tempNext = nullptr;
	
	while (true)
	{
		tempHead = head.load(std::memory_order_acquire);
		tempNext = tempHead->next.load(std::memory_order_acquire);
		if (head.load(std::memory_order_relaxed) != tempHead)
			continue;

		//if (tempNext == nullptr)
		//	return NULL;	//queue only has one element


		Node* tempTail = tail.load(std::memory_order_acquire);	//might place this somewhere else

		//I really like the below, will go over in more detail
			//not liking it so much anymore
		if (tempHead == tempTail)
		{
			//Thread is needed to help enqueue
			tail.compare_exchange_strong(tempTail, tempNext, std::memory_order_release, std::memory_order_relaxed);
			continue;
		}
		if (head.compare_exchange_strong(tempHead, tempNext, std::memory_order_release, std::memory_order_relaxed))
			break;
	}

	int tempData = tempHead->data;
	numNodes.fetch_sub(1, std::memory_order_acq_rel);
	free(tempHead);

	return tempData;
}

int SPSCQueue::size()
{
	return numNodes;
}
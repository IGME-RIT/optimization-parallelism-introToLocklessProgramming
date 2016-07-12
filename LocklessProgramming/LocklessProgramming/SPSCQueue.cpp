#include "ClassDeclarations.h"

//SO first lets talk about some design decisions
//first we will go with a stereotypical FIFO ordering, just because ...
//second since this is a Single Producer and Single Consumer Queue,
	//the biggest things we have to watch out for are when a enqueue and dequeue are happenening simutaneouly and there are 2 < nodes
//A Single Producer and Single Consumer Queue is a queue where only 1 thread will enqueue(producer) and only 1 thread will dequeue(consumer)
	//This is a very important, since this means that each thread is operating on seperate parts of the queue making it so that
//Also it is implemented as a Singly Linked List
	//I did this because it helps show some of the differences between the a locked vs lockless programming and it is the easiest implementations to understand

//A version of the constructor that initializes the list with a single node
	//Assuming initialization will be inherantly outside a multithreaded environment
SPSCQueue::SPSCQueue(int data)
{
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = nullptr;
	//remember that this is being created outside a multithreaded environment and so, we don't care about memory reordering
	head.store(newNode, std::memory_order_relaxed);
	tail.store(newNode, std::memory_order_relaxed);
	closing.store(false, std::memory_order_relaxed);
	numNodes.store(1, std::memory_order_relaxed);
}

//Closing flag might be superflorous
SPSCQueue::~SPSCQueue()
{
	//We made closing an atomic
		//so it's position can't be reordered and so other threads
		//use it and it can create a sync point with its memory ordering
	closing.store(true, std::memory_order_release);
	//We use the relaxed memory ordering, since it doesn't matter with the release memory order used by closing
	int temp = numNodes.load(std::memory_order_relaxed);
	for (int i = 0; i < temp; i++)
	{ 
		dequeue();
		//printf("Num: %d\n", dequeue());
	}

	Node* tempTail2 = back();
}

//This is just an encapsulated load of head that turns it into a Node* so you can see the values of the Node's variables in the debugger
Node* SPSCQueue::front()
{
	return head;
}

//Has the same basic reason behind it as front()
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
		//enqueue does not deal with the case of nothing in the queue
			//either make it so dequeue never dequeue's the whole thing or
			//make the enqueue be able to set the head node
		//there could be problems with the while loop, since if there is a problem with the tail it doesn't update it

//Enqueue will add a Node to the queue
	//might not need to return a bool
bool SPSCQueue::enqueue(int data)
{
	//Also, I decided that the enqueue function
	//First we will allocate the new node and set it up
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = nullptr;
	
	Node* tempTail;
	Node* tempNext;
	Node* temp = nullptr;

	//might want to make some changes to the loop
	while (true)
	{
		//first load tail innto a temporary variable
		tempTail = tail.load(std::memory_order_acquire);
		//Then load it's next pointer into a temporary variable
		tempNext = tempTail->next.load(std::memory_order_acquire);
		//We are checking the next pointer to make sure it is still null, if it isn't then enter the if statment
			//since this means that the current tail pointer is wrong
		if (tempNext != nullptr)
		{
			//change the TempTail to to tempNext
			tail.compare_exchange_weak(tempTail, tempNext, std::memory_order_release, std::memory_order_relaxed);
		}

		temp = nullptr;
		//check to make sure that tempTail->next is still a nullptr
		//if it is store the new node's pointer into the current tails next pointer and then break out of the loop
			//(remember tempTail is suppose to be a pointer to the current tail)
		//if tempTail->next is not the current tail redo the loop
		if (tempTail->next.compare_exchange_strong(temp, newNode, std::memory_order_release, std::memory_order_relaxed))
			break;
	}

	//Set the current tail to the new Node, but compare the tempTail to the current tail to make sure this is okay
	tail.compare_exchange_strong(tempTail, newNode, std::memory_order_acq_rel, std::memory_order_relaxed);
	//The update the number of nodes
		//might want this before the other, but probably not
	numNodes.fetch_add(1, std::memory_order_release);	//not sure if I need the std::memory_order_release, might just need relaxed
	return true;
}

//Enqueue will remove a Node to the queue
int SPSCQueue::dequeue()
{
	Node* tempHead = nullptr;
	Node* tempNext = nullptr;
	Node* tempTail = nullptr;
	
	while (true)
	{
		//the the current head node into a temporary variable
		tempHead = head.load(std::memory_order_acquire);

		//check to see if the queue has zero elements
			//if so return NULL
		if (tempHead == nullptr)
			return NULL;	//queue has zero elements

		//load the head's next variable into a temporary variable
		tempNext = tempHead->next.load(std::memory_order_acquire);
		//if the current head doesn't equal the temporary variable, go to the next iteration of the loop
		if (head.load(std::memory_order_relaxed) != tempHead)
			continue;

		//load the tail into a temporary variable
		tempTail = tail.load(std::memory_order_acquire);	//might place this somewhere else

		//I really like the below, will go over in more detail
			//not liking it so much anymore
		//if the head and the tail are the same variable
		if (tempHead == tempTail)
		{
			//Thread is needed to help enqueue
			//Compare the current tail with the temporary one to see if it has changed
				//if so, change tempTail and go to the next iteration of the loop
				//else store tempNext as the current tail variable
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
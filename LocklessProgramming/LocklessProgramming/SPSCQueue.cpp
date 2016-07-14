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
		//Due to the release and acquire semantics used the load of numNodes will always occur after closing.store()
		//Also, all threads will check if closing is true and if so, they will not continue using the queue, since this means the queue is being destroyed
	closing.store(true, std::memory_order_release);
	//We use the acquire memory ordering, so that it doesn't get reordered before the closing store
	int temp = numNodes.load(std::memory_order_acquire);
	for (int i = 0; i < temp; i++)
	{ 
		//Since we can assume that nothing can access the queue right now, we don't have to worry about memory reordering or data races below
			//thus the std::memory_order_relaxed
			//Also this won't be reordered, since that would change how it would act if run in a singly threaded environment
		Node* temp = head.load(std::memory_order_relaxed);
		head.store(temp->next, std::memory_order_relaxed);
		free(temp);
	}

	Node* tempTail2 = back();
}

//This is just an encapsulated load of head that turns it into a Node* so you can see the values of the Node's variables in the debugger
	//the debugger doesn't know how to translate atomic objects' _My_val_ for non basic types
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
	//Made some modifications

//Also Used:
	//https://www.research.ibm.com/people/m/michael/podc-1996.pdf

//Enqueue will add a Node to the queue
	//might not need to return a bool
bool SPSCQueue::enqueue(int data)
{
	//First we will allocate the new node and set it up
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = nullptr;
	
	Node* tempTail = nullptr;
	Node* tempNext = nullptr;
	Node* temp = nullptr;
	bool empty = false;	//probably don't need, since tempTail can't be updated by any other thread

	//might want to make some changes to the loop
	while (true)
	{
		//first load tail innto a temporary variable
		tempTail = tail.load(std::memory_order_acquire);

		if (tempTail == nullptr)
		{
			//empty = true;
			break;
		}

		//Then load it's next pointer into a temporary variable
		tempNext = tempTail->next.load(std::memory_order_acquire);
		
		//set temp to nullptr here for the second if statement
		temp = nullptr;

		//We are checking the next pointer to make sure it is still null, if it isn't then enter the if statment
			//since this means that the current tail pointer is wrong
		if (tempNext != nullptr)
		{
			//compare the current tail to TempTail
				//if it succeeds change the current tail to tempNext
				//else store the current tail in tempTail
					//the storing of the current tail in tempTail doesn't really matter 
			tail.compare_exchange_weak(tempTail, tempNext, std::memory_order_release, std::memory_order_relaxed);
		}
		
		//check to make sure that tempTail->next is still a nullptr
			//if it is store the new node's pointer into the current tails next pointer and then break out of the loop
				//(remember tempTail is suppose to be a pointer to the current tail)
			//if tempTail->next is not the current tail redo the loop

		//Essentially the below operation is trying to do:
			/*
			if(tempTail->next == temp)
			{
				tempTail->next = newNode;
				break;
			}
			else
				temp = tempTail->next;
			*/
		if (tempTail->next.compare_exchange_strong(temp, newNode, std::memory_order_release, std::memory_order_relaxed))
			break;
	}

	//Set the current tail to the new Node, but compare the tempTail to the current tail to make sure this is okay
	//Essentially the below operation is trying to do:
	/*
	if(tail == tempTail)
		tail = newNode;
	else
		tempTail = tail;
	*/
	tail.compare_exchange_strong(tempTail, newNode, std::memory_order_acq_rel, std::memory_order_relaxed);

	//if the thread was empty and the above will not change tempTail in the situation where the queue is empty
	if (tempTail == nullptr) //if (empty)
	{
		//Compare the value of head with temp which is a nullptr, if head == nullptr, then store the newNode as the head node, otherwise save the current head to temp
		//Essentially the below operation is trying to do:
		/*
		if(tempTail->next == temp)
		{
		tempTail->next = newNode;
		break;
		}
		*/
		head.compare_exchange_strong(temp, newNode, std::memory_order_release, std::memory_order_relaxed);
	}

	//The update that changes the number of nodes
	numNodes.fetch_add(1, std::memory_order_relaxed);
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
		tempHead = head.load(std::memory_order_relaxed);
		//load the tail into a temporary variable
		tempTail = tail.load(std::memory_order_acquire);
		
		//Does it matter if they are a big if else if set or just a bunch of if statements

		//check to see if the queue has zero elements
			//if so return NULL
		if (tempHead == nullptr)
			return NULL;	//queue has zero elements

		//if the current head doesn't equal the temporary variable, try the loop again
		if (tempHead != head.load(std::memory_order_relaxed))
			continue;

		//load the head's next variable into a temporary variable
		tempNext = tempHead->next.load(std::memory_order_acquire);

		if (tempHead == tempTail)
			tail.compare_exchange_strong(tempTail, nullptr, std::memory_order_acq_rel, std::memory_order_relaxed);	//might cause issues

		//Now compare the current head to the temporary variable tempHead, to make sure it doesn't change
			//if it didn't, save tempNext into the head (basically head = head->next), and then break out of the loop
			//else save tempNext into the current head and break out of the loop

		//The below is essentially:
			/*
			if(tempHead == head)
			{
				head = head->next;
				break;
			}
			*/
		if (head.compare_exchange_strong(tempHead, tempNext, std::memory_order_release, std::memory_order_relaxed))
			break;
	}

	//subtract one from the number of nodes, to signal to everything that head is officially gone
	numNodes.fetch_sub(1, std::memory_order_acq_rel);
	//save the data to return in a temp variable
	int tempData = tempHead->data;
	
	//Now that we have updated the queue and saved the data to return, we can finally free tempHead (aka the old head)
	free(tempHead);

	//return the data and we are done
	return tempData;
}

int SPSCQueue::size()
{
	//Just a function to encapsulate numNodes, so the queue is the only one that can mess with it
	return numNodes;
}
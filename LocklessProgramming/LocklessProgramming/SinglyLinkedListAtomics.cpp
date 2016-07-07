#include "SinglyLinkedListAtomics.h"

static slist l;

//need to work on
void slist::push(int val)
{
	Node* temp = head;
	Node* newNode = new Node{ val, temp };

	while (!head.compare_exchange_weak(temp, newNode))
	{
		newNode->next = temp;
	}
	size.fetch_add(1);
}

void foo(int num)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	l.push(num);
}

static const int s = 100;

void addToLL()
{
	std::thread threads[s];
	for (int i = 0; i < s; i++)
	{
		threads[i] = std::thread(foo, i);
	}

	for (int i = 0; i < s; i++)
	{
		if (threads[i].joinable())
			threads[i].join();
	}

	Node* temp = l.head;

	for (int i = 0; i < l.size; i++)
	{
		std::cout << "Value of Head Node: " << temp->value << std::endl;
		temp = temp->next;
	}
	/*std::cout << "Value of Head Node: " << ((Node*)l.head._My_val)->value << std::endl;
	std::cout << "Value of Head Node: " << ((Node*)l.head._My_val)->next->value << std::endl;
	std::cout << "Value of Head Node: " << ((Node*)l.head._My_val)->next->next->value << std::endl;*/
}


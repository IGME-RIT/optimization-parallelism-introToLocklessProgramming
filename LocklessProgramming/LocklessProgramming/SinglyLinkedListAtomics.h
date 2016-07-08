#pragma once
#include <atomic>
#include <thread>
#include <iostream>

struct NodeN
{
	int value;
	NodeN* next;
};

//Lockfree Data Structures
class slist
{
public:
	std::atomic<NodeN*> head = nullptr;
	//Making size an atomic to make sure that the size write always happens after the compare_exchange_weak()
		//Also since size++ isn't atomic on all architectures
	std::atomic<unsigned int> size = 0;
	void push(int val);
};

void addToLL();
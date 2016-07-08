#pragma once
#include <atomic>
#include <thread>
#include <cstdint>

class Atomics
{
public:
	void examples();
};

#define QUEUE_MAX 16

struct Node
{
	int data;
	Node* next;
	Node* prev;
};

//FIFO ordering
class SPSCQueue
{
public:
	SPSCQueue();
	~SPSCQueue();
	bool enqueue(int data);
	int dequeue();
	uint32_t size();
private:
	std::atomic<uint32_t> numNodes;
	Node* head;
	Node* tail;
};
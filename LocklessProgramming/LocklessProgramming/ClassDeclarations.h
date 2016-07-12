#pragma once
#include <atomic>
#include <thread>
#include <cstdint>
#include <mutex>

class Atomics
{
public:
	void examples();
};

struct Node
{
	int data;
	std::atomic<Node*> next;
};

//FIFO ordering
class SPSCQueue
{
public:
	std::atomic<bool> closing;
	
	//A constructor that takes in a value and makes sure the queue is initialized with a single element at creation
	SPSCQueue(int data);
	~SPSCQueue();
	bool enqueue(int data);
	int dequeue();
	int size();

	//Encapsulate the head and tail nodes for easy debugging
	Node* front();
	Node* back();

private:
	std::atomic<uint32_t> numNodes;
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
};

struct LockedNode
{
	int data;
	LockedNode* next;
};

class LockedSPSCQueue
{
public:
	//A constructor that takes in a value and makes sure the queue is initialized with a single element at creation
	LockedSPSCQueue(int data);
	~LockedSPSCQueue();
	bool enqueue(int data);
	int dequeue();
	int size();
private:
	std::mutex mtx;
	uint32_t numNodes;
	LockedNode* head;
	LockedNode* tail;
};
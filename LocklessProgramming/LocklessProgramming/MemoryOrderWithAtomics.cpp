#include "ClassDeclarations.h"

//Look into:
	//http://www.1024cores.net/
	//http://moodycamel.com/blog/2013/a-fast-lock-free-queue-for-c++
	//https://www.justsoftwaresolutions.co.uk/threading/intel-memory-ordering-and-c++-memory-model.html

//This one is very good.
	//https://bartoszmilewski.com/2008/12/01/c-atomics-and-memory-ordering/

//Now lets talk about memory ordering in std::atomic
//I talked briefly about memory ordering for Atomics in Atomics.cpp

//Below are the possible memory orders that you can give C++11 Atomic Objects and C++11 atomic_thread_fence()
//I will cover the use of memory orders in relation to C++11 Atomic Objects and I will be ignoring C++11 atomic_thread_fence()

//std::memory_order enum - possible values:
	//memory_order_relaxed,   // relaxed - ordered to happen atomically at some point
	//memory_order_consume,   // consume - ordered to happen once all accesses to memory in the releasing thread that carry a dependency on the releasing operation have happened
	//memory_order_acquire,   // acquire - ordered to happen once all accesses to memory in the releasing thread have happened
	//memory_order_release,   // release - ordered to happen before a consume or acquire operation serving as a sync point for other accesses to memory that may have visible side effects on the loading thread
	//memory_order_acq_rel,   // acquire/release - the operation loads acquiring and stores releasing
	//memory_order_seq_cst    // sequentially consistent - ordered in a sequentially consistent manner
		//All operations using this memory order are ordered to happen once all accesses to memory that may have visible side effects on the other threads involved have already happened.
		//strictest memory order
		//for consume and acquire loads, these store operations are considered releasing operations
		//All C++11 Atomics use this as the default

//This can be

//The really important ones are below:
	//std::memory_order_acquire
	//std::memory_order_release
	//std::memory_order_acq_rel
	//std::memory_order_relaxed

//A quick note, these memory orderings act like what I would call a synchronization point with C++11 atomic objects
	//they sync the various operations on the atomic objects with the other operations
	//creating a point in execution where one must happen before the other, which is the point of these memory orders with C++11 atomic objects

//They also act like fences and can create fences underneath the hood if used with an atomic object's member function

//Now lets talk about them

static void func()
{
	//Two atomic variables to use
	std::atomic<int> num1 = 1;
	std::atomic<int> num2 = 2;
	int temp = 0;

	//First let's cover - std::memory_order_release
		//This is used to make sure a store happens before a load operation 
		num1.store(5, std::memory_order_release);
		temp = num1.load(std::memory_order_acquire);
		//The release operation is required to happen first, before an acquire operation and in this way you prevent the memory reordering

	//First lets cover - std::memory_order_acquire
		//This is used when you want to make sure a load operation happens after a release operation
		//Example:
		num1.store(5, std::memory_order_release);
		temp = num1.load(std::memory_order_acquire);
		//So first 5 is stored into num1
		//Then since num1's load operation has a std::memory_order_acquire it is required to happen after num1's store operation since it has the std::memory_order_release memory order

	//Now that we have learned about std::memory_order_release and std::memory_order_acquire, we can talk about the next one - std::memory_order_acq_rel
		//This is a combination of both std::memory_order_release and std::memory_order_acquire and mainly used for operation that both require loading and storing a variable
		//Example:
			num1.compare_exchange_strong(temp, 1, std::memory_order_acq_rel, std::memory_order_relaxed);
			//Since compare_exchange_strong() both has to load, compare, and then swap information it probably should have a memory ordering std::memory_order_acq_rel for the success state, since there is a load and store there


	//Is the below true????
				//Pretty sure

	//Lets cover the easiest one to talk about - std::memory_order_relaxed
		//This memory order literally means that you don't care about that atomics memory ordering
		//This is useful when you already have an atomic with a memory ordering that already prevents the current atomic from being reodered
		//Example:
			temp = num1.load(std::memory_order_relaxed);
			temp = num2.load(std::memory_order_acquire);
			//There is no reason to put std::memory_order_acquire on num1's load, since num2's load guarantees that num2's load will happen once all accesses to memory have happened (aka num1's load)

	//Overall the above is showing off the acquire and release semantics
	//To see how to use them in action: http://preshing.com/20121019/this-is-why-they-call-it-a-weakly-ordered-cpu/
}
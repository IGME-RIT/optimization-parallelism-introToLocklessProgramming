#include <atomic>

//https://www.youtube.com/watch?v=c1gO9aB9nbs - for a C++ general community lockless programming
	//issue - said something about std::memory_order being useless should be wrong, be careful
//https://www.youtube.com/watch?v=X1T3IQ4N-3g - for more game style parallelism

//look at: https://www.youtube.com/watch?v=JUeaCfhwvHE

//fences
	//acquire
	//release

//Two Basic Tools
	//Think in transactions (ACID)
		//Atomicity
			//A transaction is all-or-nothing, other code must not be able to see the data in a partially-updated state
		//Consistency, Isolation, Durability
			//A transaction takes data from one consistent state to another
			//2 transactions never simultaneously operate on the same data
			//A commited transaction is never overwritten by a second transaction that did not see the result of the first transaction
		//Make sure that updates don't interfere with each other 
	//atomics
		//std::atomic<T>
		//Each individual read and write is atomic no locking required
		//Read/Writes are guaranteed not to be reordered
		//The Compare-and-Swap - in std::atomic it is compare_and_exchange_weak or compare_and_exchange_strong
			//Conceptually
			/*bool atomic<T>::compare_exchange_strong(T& expected, T desired)
			{
				if(this->value == expected) { this->value = desired; return true; }
				else						{ expected = this->value; return false; }
			}*/
			//It looks like it does all of this in one operation
		//don't worry about the memory order parameter
		//Remember to explicity initialize atomics


	//Three Levels of Lock-free
		//Wait-free:
			//Every operation will complete in a constrainted number of steps no matter what
			//nothing ever waits
			//Guaranteed to be Starvation Free
		//Lock-free
			//Every step taken achieves global progress
			// Is NOT guaranteed to be Starvation Free
		//Obstruction-free
			//weakest
			//at any point a single thread executed in isolation for a contrainted number of steps will complete its operation
			//Does not guarantee progress
			//Guarantee Deadlock is impossible, does not guarantee that livelock is impossible

		//Double Checked Locking
			//Correct and slighlty simplistic:
			/*
			atomic<Widget*>Widget::pinstance{nullptr};
			Widget* Widget::Instance()
			{
				//atomic read
				//then compare it against nullptr
				if(pinstance = nullptr)
				{
					//if it is nullptr, lock it
					lock_guard<mutex> lock{mut};
					//check to make sure pinstance is still null
						//In case many threads are trying this exact same method at the same time
					if(pinstance == nullptr)
					{
						pinstance = new Widget();
					}
				}
				return pinstance;
			}
			*/
			//Faster Version:
			/*
			atomic<Widget*>Widget::pinstance{nullptr};
			Widget* Widget::Instance()
			{
				//Load once and then just reuse it
				//Gets rid of an extra load operation
				Widget* p = pinstance;
				if(p = nullptr)
				{
					//if it is nullptr, lock it
					lock_guard<mutex> lock{mut};
					//check to make sure pinstance is still null
					//In case many threads are trying this exact same method at the same time
					if((p == pinstance) == nullptr)
					{
						pinstance = p = new Widget();
					}
				}
				return p;
			}
			*/

			//Meyers Singleton
			//Static - thread safe initialized function local statics
				//does double check locking or something similar under the covers
			//2014 talk
			/*
			widget& widget::get_instance()
			{
				static widget instance;
				return instance;
			}
			*/

			//These vary based on architecture, but as a general rule of thumb
				//Stores have overhead
				//Load has a small amount of overhead

//Concurrent Objects - multiple threads operate concurrently on the object with at least one thread modifying its state
	//Had to write concurrent objects on top of
		//platform primitives - mutexes, semaphores, event objects
		//atomic operations - used when the object is under very heavy contention

//3 Threading Patterns
	//1) Pipelining Work
		//Can be done with a semaphore
		//how to avoid race conditions/ concurrent modifications?
			//Double-Buffered Graphics State
				//Double buffer any state that is relavent to both threads
				//have two copies of an object's position matrix
			//Seperate Graphic Objects
				//Have a graphic object version and a engine object version
				//Copy the needed state at the beginning of each frame
	//2) Dedicated Threads
		//Going to use Content Streaming as an example
		//Have an Engine Thread
		//Have a loading thread
			//have some CPU work
		//use a concurrent queue
			//not high contension, so you can just through a lock on the object
			//How to improve on the queue
				//allow canceling requests, interrupt requests, and re-prioritize requests
					//makes it more of a custom object
		//Put the loading thread to sleep when done
			//Tend to use an events for this
				//when event is signaled the threads pass through
				//when the event is reset/not signaled the theads wait
			//each time somebody submits work to the requests queue
				//signals that event
				//loading thread waits on that event at the beginning of each iteration of its outer loop
				//when it gets the signal it resets it right away
				//does as much work as possible exhausting the queue
	//3) Task Schedulers
		//Born to get more fine grained parrallelism
		//space it across several worker threads
			//dedicated to doing tasks
		//use a concurrent queue
		//each worker thread would have its own event to deal with waking it up
		//Organised tasks into groups, example:
			//Input, Logic, Physics, Animation
			//Each Task Group keeps an array of items to update in parallel
			//Allow multiple threads to work on the same Task Group

std::atomic<int> p;

void foo()
{
}
#include "ClassDeclarations.h"
#include <iostream>
//How to correctly use atomic variables
//These are all atomic operations

//So we talked briefly about Atomics in my last tutorial, but we really didn't get into the meat of C++11 Atomics
//Now we will take a more indepth look at them

//Below is a list of important functions that C++11 Atomic Objects have

/*p.compare_exchange_strong(T& expected, T val); - compares and exchanges the contents of the contained value with expected
	//if true - replaces the contained value with val
	//if false - replaces expected with the contained value
	//unlike the weak version, this function makes sure to always return true when expected compares equal to the contained object
	//the weak version may produce significantly better performance
p.compare_exchange_weak(T& expected, T val); - compares the contents of the atomic object's contained value with expected
	//if true - replaces the contained value with expected
	//if false - replaces expected with the contained value
	//unlike the strong version, this function is allowed sometimes return false even when expected compares equal to the contained object and doesn't modify expected when this happens
p.exchange(); - replaces the contained value with val and return the value it had before
p.fetch_add(); - Add val to the contained value and returns the value it had immediately before the operation
p.fetch_sub(); - subtracts val from the contained value and returns the value it had immediately before the operation
p.fetch_and(); - reads the contained value and replaces it by the result of performing a bitwise AND operation between the read value and val
p.fetch_or(); - reads the contained value and replaces it by the result of performing a bitwise OR operation between the read value and val
p.fetch_xor(); - reads the contained value and replaces it by the result of performing a bitwise XOR operation between the read value and val
p.is_lock_free(); - indicates whether the object is lock-free
p.load(); - returns the contained value
p.store(); - replaces the contained value with val*/

//These are important, but we will go through how to use them below, so if you skipped the giant wall of text that's okay

//Now a great thing about atomics is that you can specify memory orders
//There are several, but they are easy to understand
//We won't go into them too heavily, since it would be too much to cover in this cpp file
	//To truly understand them to there fullest, you will need to read MemoryOrderWithAtomics.cpp and understand the memory orderings

//Remember the std::atomic is not guaranteed to be lockfree on every platform
	//you can call .is_lock_free() to check if the particular variable is lockfree
		//Example: sharedNum.is_lock_free();

static void t1Fun();
static void t2Fun();
static void t1FunV2();
static void t2FunV2();

static std::atomic<int> sharedNum = 0;

void Atomics::examples()
{
	printf("Macro: %d\n", ATOMIC_INT_LOCK_FREE);

	//seeding the rand() with the time
	srand(time(NULL));

	//Just making a basic atomic variable called num
	std::atomic<int> num;

	//first lets show all the ways to store in a sequentially consistent manner
	
	//Since num is the atomic object wrapping an integral data type,
		//you probably remember that the below can be done atomically
	num = 5;
	//Remember that the above is assuming a sequentially consistent ordering

	//Now lets see how to do this with one of std::atomic's built in functions
	num.store(5);
	//This does the exact same thing as num = 5; They are exactly equivalent in this case

	//Now for the final way to do the above operations
	num.store(5, std::memory_order_seq_cst);
	//Now this code is the same as everything else above it


	//Now lets show all the ways to load in a sequentially consistent manner

	//Again remembering how to do a basic load operation with an std::atomic
	int temp = num;
	//This is atomic if you remember and also uses memory order std::memory_order_seq_cst

	//Now lets see the built in function for this:
	temp = num.load();
	//again this does same as the code above it

	//The below is the exact same as above
		//atomics use std::memory_order_seq_cst
	temp = num.load(std::memory_order_seq_cst);
	//again, nothing has changed

	//Continuing on to some of the other uses atomics have:

	//Below we are just incrementing num
	//since num is an atomic object, the below is atomic
		//remember the below operation has std::memory_order_seq_cst by default
	num++;

	//The above is the same functionally as the below, which is also guarantee to be atomic
		//Again the below operation has std::memory_order_seq_cst by default
	num += 1;

	//which is functionally the same as
		//Again the below operation has std::memory_order_seq_cst by default
	num.fetch_add(1);

	//which is finally functionally the same as
		//Again the below operation has std::memory_order_seq_cst by default
	num.fetch_add(1, std::memory_order_seq_cst);






	//Now those last do have very slightly different assemblty instruction from the others, but overall it isn't that different???????????? need to just read more about







	//All the above do is add one to num in a sequentially consistent matter
	//Now why is this important, because the above show all the different ways atomic objects can function
	//They all overload the increment and decremenet operators(--, ++) and the compound assignment operators(+=, -=, etc) and even have a function version of the compound assignment operators that can take a memory ordering

	//Now why are we going over this?
	//Well to show all the different ways std::atomics can be used, but also
		//Well, that is because we can pass in a std::memory_order enum to specify how to handle memory ordering

	//An Example of one:
	std::atomic<int> num2 = 0;
	//Now we can store it using a different memory ordering
	//num2.store(2, std::memory_order_release);
	//This can be used to control the memory order of reads and and writes, similar to how fences work

	//Now lets show an example of how to use these in a multithreaded environment

	//Create 2 threads and give each a function
	//t1 will modify the variable
	std::thread t1(t1FunV2);
	//t2 will read and print the variable
	std::thread t2(t2FunV2);

	//wait for both threads to finish
	t1.join();
	t2.join();


	//now time to talk about the other functions atomic has that are important
	//fetch functions
		//std::atomics also have a bunch of fetch functions that load the variable and then do something to it
			//These include add, subtract, and the bitwise operations(AND, OR, AXOR)
			//Example:
			printf("Value of Num2: %d\n", num2.load(std::memory_order_acquire));
			num2.fetch_add(1, std::memory_order_acq_rel);
			printf("Value of Num2 After fetch_add(): %d\n", num2.load(std::memory_order_acquire));
				//since this is a Read-Modify-Write operation, I'm using the above memory order since it has both a acquire and release operation

	//exchange()
		//std::atomics have an exchange function, it is a Read-Modify-Write operation, it just replaces the atomic variables value and returns the previous value
			//Example for use from: http://www.cplusplus.com/reference/atomic/atomic/exchange/
			//Other Example:
			int exchangedNum = num2.exchange(6);
			//printf("Exhanged Number: %d\nCurrent Value: %d\n", exchangedNum, num2.load(std::memory_order_acquire));

			printf("Lock free: %d\n", num2.is_lock_free());

	//compare_exchange functions
		//These functions are super important as they are a key component of lockless programming
		//They take in a desired value and a expected value
		//Then they check if the expected value is the same as the current stored value
			//if it IS it replaces the current value with the desired value
			//if it ISN'T it writes over the expected value with the current value
		//This is an implementation of a commonly refered to operation in lockless programming:
			//The compare and swap operation, abbreviated CAS
		//There are two versions of this function the weak version and the strong version
			//the strong version guarantees that this will happen
			//the weak version may fail
		//The parameters take a reference to a expected value, a desired value, memory order in case of expected value = current, memory in case of expected value != current
		//Example:
			//Load num2 into temp2, since we need an expected value
			int temp2 = num2.load(std::memory_order_acquire);
			//print current value of temp2/num2
			printf("Temp2: %d\n", temp2);
			//Now try to compare and exchange
			num2.compare_exchange_weak(temp2, 9, std::memory_order_acq_rel, std::memory_order_relaxed);
			
			num2.compare_exchange_strong(temp2, 9, std::memory_order_acq_rel, std::memory_order_relaxed);
			printf("Temp2 After 2nd CAS: %d\n", temp2);
}


//The below will always print 0 or 41 for some reason

static void t1Fun()
{
	//something to hold the random int
	//create outside the loop so we aren't creating it 2000 times
	int randNum = 0;
	for (int i = 0; i < 2000; i++)
	{
		//get a random number
		randNum = rand() % 100;
		//Now we will atomically store that number into sharedNum
		//
		sharedNum.store(randNum, std::memory_order_release);
	}
}

static void t2Fun()
{
	for (int i = 0; i < 5; i++)
	{
		//Notice how below we aren't worried about the standard output buffer being shared or race conditions happening with the standard output buffer
		//This is because this is the only thread using it, so it makes sense that since the standard output buffer is only being used on a single thread that we don't need to worry about the printf statement

		//Now we are running this loop 5 times, merely so that we can have a thing where one thread needs the variable to store something and another thread needs the variable to read it
		//But it doesn't matter since both operations are atomic
		//Neither thread will be able to see the other operation
			//Means no Race Conditions!! Woo!

		//Now why do we use std::memory_order_acquire

		printf("Shared Number: %d\n", sharedNum.load(std::memory_order_acquire));
	}
}

//Although remember depending on the architecture you might not need atomics
	//The below code will run perfectly in a multithreaded environment if you make sure an int's store and load operations are atomic for your architecture
	//I'm running this on a x86 architecture with the fact that an int is 32 bits

static int sharedNum2;

static void t1FunV2()
{
	//something to hold the random int
	//create outside the loop so we aren't creating it 2000 times
	int randNum = 0;
	for (int i = 0; i < 2000; i++)
	{
		//get a random number
		randNum = rand() % 100;
		//Now we will atomically store that number into sharedNum
		//
		sharedNum2 = randNum;
	}
}

static void t2FunV2()
{
	for (int i = 0; i < 5; i++)
	{
		//Notice how below we aren't worried about the standard output buffer being shared or race conditions happening with the standard output buffer
		//This is because this is the only thread using it, so it makes sense that since the standard output buffer is only being used on a single thread that we don't need to worry about the printf statement

		//Now we are running this loop 5 times, merely so that we can have a thing where one thread needs the variable to store something and another thread needs the variable to read it
		//But it doesn't matter since both operations are atomic
		//Neither thread will be able to see the other operation
		//Means no Race Conditions!! Woo!

		//Now why do we use std::memory_order_acquire

		printf("Shared Number: %d\n", sharedNum2);
	}
}

//For some more info on memory ordering and a different explanation read:
	//https://bartoszmilewski.com/2008/12/01/c-atomics-and-memory-ordering/
//And the follow up post: https://bartoszmilewski.com/2008/12/23/the-inscrutable-c-memory-model/

//good to remember:
	//Btw, you can use relaxed stores in class constructors, because initially object is not shared.
	//And sharing/publication mechanism will provide all necessary synchronization.
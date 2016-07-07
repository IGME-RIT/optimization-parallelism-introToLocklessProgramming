#include <atomic>
#include <thread>

//std::memory_order enum - possible values
//memory_order_relaxed,   // relaxed - ordered to happen atomically at some point
//memory_order_consume,   // consume - ordered to happen once all accesses to memory in the releasing thread that carry a dependency on the releasing operation have happened
//memory_order_acquire,   // acquire - ordered to happen once all accesses to memory in the releasing thread have happened
//memory_order_release,   // release - ordered to happen before a consume or acquire operation serving as a sync point for other accesses to memory that may have visible side effects on the loading thread
//memory_order_acq_rel,   // acquire/release - the operation loads acquiring and stores releasing
//memory_order_seq_cst    // sequentially consistent - ordered in a sequentially consistent manner
//All operations using this memory order are ordered to happen once all accesses to memory that may have visible side effects on the other threads involved have already happened.
//strictest memory order
//for consume and acquire loads, these store operations are considered releasing operations

//Example:
//std::memory_order p = std::memory_order_acquire;

void function()
{
	//s
}
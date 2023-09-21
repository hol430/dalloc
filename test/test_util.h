#ifndef _DALLOC_TEST_UTIL_H_
#define _DALLOC_TEST_UTIL_H_

#include <stdint.h>

typedef void (*signal_handler_t)(int32_t signum);

typedef void *(*_sbrk_handler_t)(intptr_t ptr);

void attach_signal_handler(int32_t signum, signal_handler_t handler);

/*
Detach all signal handlers.
*/
void detach_signal_handlers(int32_t signum);

/*
Add a custom sbrk implementation to replace all dalloc internal calls to sbrk,
until remove_sbrk_handlers() is called.
*/
void attach_sbrk_handler(_sbrk_handler_t handler);

/*
Remove all custom sbrk implementations.
*/
void remove_sbrk_handlers();

/*
Assert that the first size bytes of ptr0 and ptr1 are equal.

@param size: Number of bytes to check.
@param ptr0: A pointer.
@param ptr1: A pointer.
*/
void assert_ptr_contents_equal(size_t size, void *ptr0, void *ptr1);

/*
Fill the first size bytes of memory starting at ptr0 with non-random data.

@param size: Number of bytes to write.
@param ptr: The pointer.
*/
void fill_memory(size_t size, void *ptr);

#endif // _DALLOC_TEST_UTIL_H_

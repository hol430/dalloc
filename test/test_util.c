#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include "test_util.h"

void attach_signal_handler(int32_t signum, signal_handler_t handler) {
	// Inside your test case
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(signum, &sa, NULL);
}

void detach_signal_handlers(int32_t signum) {
	// Remove the custom signal handler for SIGILL
	struct sigaction sa;
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(signum, &sa, NULL);
}

static _sbrk_handler_t __dalloc_sbrk = NULL;

void *_sbrk(intptr_t ptr) {
	// Use custom sbrk if provided.
	if (__dalloc_sbrk) {
		return __dalloc_sbrk(ptr);
	}
	// Use system sbrk.
	return sbrk(ptr);
}

void attach_sbrk_handler(_sbrk_handler_t handler) {
	__dalloc_sbrk = handler;
}

void remove_sbrk_handlers() {
	__dalloc_sbrk = NULL;
}

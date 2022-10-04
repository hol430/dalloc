#include <stdbool.h>

#include "dalloc_config.h"

bool robust_mode() {
#if DALLOC_ROBUST_MODE == 1
	return true;
#endif
	return false;
}

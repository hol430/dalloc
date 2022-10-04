#ifndef _DALLOC_CONFIG_H_
#define _DALLOC_CONFIG_H_

#include <stdbool.h>

// Configured options and settings for dalloc
#define DALLOC_VERSION_MAJOR @DALLOC_VERSION_MAJOR@
#define DALLOC_VERSION_MINOR @DALLOC_VERSION_MINOR@

/*
Returns true iff running in robust mode.
*/
bool robust_mode();

#endif // _DALLOC_CONFIG_H_


# Add source files here.
target_sources("${test}"
	PRIVATE
		test.c
		test_malloc.c
		test_malloc.h
		test_free.c
		test_free.h
)

# Include paths
target_include_directories(
	"${test}"
	PRIVATE
		../src
)

# Linked libraries
find_library(CHECK check)
target_link_libraries(
	"${test}"
	PUBLIC
		c # link against libc.so first so we can use system malloc from the unit tests
		"${dalloc}"
		${CHECK}
)

target_link_options(
	"${test}"
	PRIVATE
		-lm
)

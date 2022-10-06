
# Add source files here.
target_sources("${test}"
	PRIVATE
		test.c
		test_malloc.c
		test_malloc.h
		test_free.c
		test_free.h
		test_heap_traversal.c
		test_heap_traversal.h
		test_heap_manip.c
		test_heap_manip.h
)

# Include paths
target_include_directories(
	"${test}"
	PRIVATE
		../src
)

target_compile_options(
	"${test}"
	PUBLIC
		--coverage
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

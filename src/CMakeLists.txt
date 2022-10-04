
configure_file(dalloc_config.h dalloc_config.h)

# Add source files here.
target_sources("${dalloc}"
	PRIVATE
		dalloc.h
		dalloc.c
		dalloc_utils.h
		dalloc_utils.c
		dalloc_heap_traversal.h
		dalloc_heap_traversal.c
		dalloc_internal.c
		dalloc_io.h
		dalloc_io.c
		chunk.h
		chunk.c
		dalloc_config.h
		dalloc_config.c
	PUBLIC
		dalloc_internal.h
)

# Include directories
target_include_directories("${dalloc}"
	PUBLIC
		${CMAKE_CURRENT_LIST_DIR}
)

target_compile_options("${dalloc}"
	PRIVATE
		-Wall -Werror -pedantic -Wno-pointer-arith)

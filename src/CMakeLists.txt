
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
		dalloc_io.h
		dalloc_io.c
		chunk.h
		chunk.c
		dalloc_config.h
		dalloc_config.c
)

# Include directories
target_include_directories("${dalloc}"
	PUBLIC
		${CMAKE_CURRENT_LIST_DIR}
)

target_compile_options("${dalloc}"
	PUBLIC
		--coverage
	PRIVATE
		-Wall -Werror -pedantic -Wno-pointer-arith)

target_link_options(
	"${dalloc}"
	PUBLIC
		--coverage
)


configure_file(dalloc_config.h.in dalloc_config.h)

# Add source files here.
target_sources("${dalloc}"
	PRIVATE
		dalloc.h
		dalloc.c
		dalloc_utils.h
		dalloc_utils.c
		dalloc_internal.c
	PUBLIC
		dalloc_internal.h
)

# Include directories
target_include_directories("${dalloc}"
	PUBLIC
		${CMAKE_CURRENT_LIST_DIR}
)

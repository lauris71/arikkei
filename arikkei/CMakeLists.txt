set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

set(ARIKKEI_HEADERS
	arikkei-cache.h
	arikkei-dict.h
	arikkei-iolib.h
	arikkei-strlib.h
	arikkei-token.h
	arikkei-url.h
	arikkei-utils.h
	tinycthread.h
)

set(ARIKKEI_SOURCES
	arikkei-iolib.c
	arikkei-strlib.c
	arikkei-cache.c
	arikkei-dict.c
	arikkei-token.c
	arikkei-url.c
    arikkei-utils.c
    tinycthread.c
)

add_library(arikkei STATIC
	${ARIKKEI_HEADERS}
	${ARIKKEI_SOURCES}
)

set_property(TARGET arikkei PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(arikkei PUBLIC
    ${PROJECT_SOURCE_DIR}
)

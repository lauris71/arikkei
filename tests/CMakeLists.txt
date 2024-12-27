set(CMAKE_BUILD_TYPE Debug)

add_executable(arikkei_test
    unity/unity.c
    test.c
)

target_compile_definitions(arikkei_test PRIVATE UNITY_INCLUDE_DOUBLE)
target_include_directories(arikkei_test PRIVATE ${CMAKE_SOURCE_DIR} ..)
target_link_directories(arikkei_test PRIVATE ${CMAKE_BINARY_DIR}/libarikkei)
target_link_libraries(arikkei_test PRIVATE arikkei)

add_test(NAME memcpy COMMAND arikkei_test memcpy)
add_test(NAME join COMMAND arikkei_test join)
add_test(NAME strtoll COMMAND arikkei_test strtoll)
add_test(NAME strtod COMMAND arikkei_test strtod)
add_test(NAME itoa COMMAND arikkei_test itoa)
add_test(NAME dtoa COMMAND arikkei_test dtoa)
add_test(NAME utf8 COMMAND arikkei_test utf8)
add_test(NAME utf16 COMMAND arikkei_test utf16)

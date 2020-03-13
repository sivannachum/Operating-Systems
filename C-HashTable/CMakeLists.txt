set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pipe -g")

add_executable(main main.c bytes.c hashtable.c)
add_executable(test_bytes test_bytes.c bytes.c)
add_executable(test_assoc test_assoc.c hashtable.c bytes.c)

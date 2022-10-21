set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,fuzzer -g")
add_executable(graph_fuzz graph_fuzz.cpp)


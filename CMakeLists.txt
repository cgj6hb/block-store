cmake_minimum_required (VERSION 2.8)
project(assignment1)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror")

# this is enough to get you started, but not quite enough to get it working out of the box
# Read some documentation, do some googling, refer to things we've given you
# cmake is popular, so someone on the internet has had your problem before

include_directories(${CMAKE_SOURCE_DIR}/include)

# already set for shared libs
add_library(bitmap SHARED src/bitmap.c)
add_library(block_store SHARED src/block_store.c)

install(TARGETS bitmap DESTINATION lib)
install(TARGETS block_store DESTINATION lib)
INSTALL(FILES include/block_store.h DESTINATION include)
INSTALL(FILES include/bitmap.h DESTINATION include)

# create a position-independent target
set_target_properties(bitmap PROPERTIES POSITION_INDEPENDENT_CODE ON)

# if the test program isn't found in the correct place, you will lose points
add_executable(${PROJECT_NAME}_test test/tests.cpp)

# Enable grad/bonus tests by setting the variable to 1
target_compile_definitions(${PROJECT_NAME}_test PRIVATE GRAD_TESTS=0)

target_link_libraries(${PROJECT_NAME}_test block_store gtest pthread bitmap)

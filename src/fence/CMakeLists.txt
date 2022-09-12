cmake_minimum_required(VERSION 3.0)
project(fence)
set(EXENAME fence)

# 1. set compilation flags
add_compile_options(-O3)

# 2. build and install
add_executable(cpu_${EXENAME} cpu_fence.cpp)
install(TARGETS cpu_${EXENAME} DESTINATION bin)

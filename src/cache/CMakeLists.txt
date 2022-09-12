cmake_minimum_required(VERSION 3.0)
project(cache)
set(EXENAME cache)

# 1. set compilation flags
add_compile_options(-O3)

# 2. build and install
add_executable(${EXENAME}_miss_latency_enhance cache_miss_latency_enhance.cpp)
install(TARGETS ${EXENAME}_miss_latency_enhance DESTINATION bin)

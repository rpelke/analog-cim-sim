cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build BNN/TNN library
project(BNN_TNN_Interface LANGUAGES CXX)
set(SOURCES
    src/interface_bnn_tnn.cpp
)
add_library(BNN_TNN_Interface SHARED ${SOURCES})
set_target_properties(BNN_TNN_Interface PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS BNN_TNN_Interface DESTINATION lib)

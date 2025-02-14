cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json EXCLUDE_FROM_ALL)

# Test options
option(LIB_TESTS "Build lib tests." OFF)

### Build BNN/TNN library ###
set(SOURCES_BNN_TNN
    src/interface_bnn_tnn.cpp
)
add_library(BNN_TNN_Interface SHARED ${SOURCES_BNN_TNN})
set_target_properties(BNN_TNN_Interface PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS BNN_TNN_Interface DESTINATION lib)

### Build Int library ###
set(SOURCES_INT
    src/interface_int.cpp
    src/helper/config.cpp
    src/mapping/crossbar_mapping.cpp
    src/xbar/digital_crossbar.cpp
    src/xbar/analog_crossbar.cpp
    src/adc/symadc.cpp
    src/adc/infadc.cpp
)
include_directories(inc extern/json/include)
add_library(INT_Interface SHARED ${SOURCES_INT})
target_link_libraries(INT_Interface nlohmann_json::nlohmann_json)
set_target_properties(INT_Interface PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(TARGETS INT_Interface DESTINATION lib)

if(LIB_TESTS)
    message(STATUS "Enabling INT tests.")
    enable_testing()
    add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.12)
project(test_behavior)

file(GLOB ${PROJECT_NAME}_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    BT::behaviortree_cpp_v3
    behavior
    component
    gtest
    gtest_main
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:component,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:behavior,INTERFACE_INCLUDE_DIRECTORIES>
)

add_test(test_behavior test_behavior)

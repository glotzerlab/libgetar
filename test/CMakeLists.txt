
add_executable(test_records test_Record.cpp)
add_test(test_records test_records)
target_link_libraries(test_records getar)
include_directories(../src)
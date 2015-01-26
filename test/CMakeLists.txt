
add_executable(test_records test_Record.cpp)
add_test(test_records test_records)
target_link_libraries(test_records getar)

add_executable(test_gtar test_GTAR.cpp)
add_test(test_gtar test_gtar)
target_link_libraries(test_gtar getar)

include_directories(../src)
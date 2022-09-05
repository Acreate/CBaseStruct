add_executable(main main.cpp main.h)
target_link_libraries(main PRIVATE DataStruct)

add_subdirectory(DataStruct)
add_subdirectory(Sqlite)

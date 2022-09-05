add_library(Sqlite INTERFACE)
target_include_directories(Sqlite INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Sqlite INTERFACE DataStruct)

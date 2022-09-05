add_executable(main main.cpp main.h)
target_link_libraries(main PRIVATE DataStruct)

add_subdirectory(BuilderEven)
add_subdirectory(DataStruct)
add_subdirectory(Sqlite)

INSTALL(TARGETS main DataStruct 
	RUNTIME DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/
	LIBRARY DESTINATION  ${CMAKE_CURRENT_BINARY_DIR}/..
	)

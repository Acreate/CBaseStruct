
ADD_LIBRARY( DataStruct SHARED
		DataStruct.h DataStruct.c
		ExternDataStruct.h )
GENERATE_EXPORT_HEADER( DataStruct BASE_NAME DataStruct)
target_compile_options( DataStruct PRIVATE "-DDllExport")
TARGET_INCLUDE_DIRECTORIES( DataStruct INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
TARGET_LINK_LIBRARIES( DataStruct PUBLIC BuilderEven )

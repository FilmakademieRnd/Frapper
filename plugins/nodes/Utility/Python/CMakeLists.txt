project(pythonnode)

set( res_header
	PythonNode.h
	PythonNodePlugin.h
	)

set( res_moc
	PythonNode.h
	PythonNodePlugin.h
	)

set( res_source
	PythonNode.cpp
	PythonNodePlugin.cpp
	)

set( res_description
	pythonnode.xml
	)

# find python 2.7
SET(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
FIND_PACKAGE( PythonNodeDeps )

if( PYTHONNODE_DEPENDENCIES_FOUND )
	
	set ( add_include_dir
		${PYTHONNODE_INCLUDE_DIRS}
	)

	set( add_link_lib 
		${PYTHONNODE_LIBRARIES}
	)
	add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
	add_definitions( -DBOOST_ALL_NO_LIB )
	#add_definitions( -DBOOST_PYTHON_STATIC_LIB )
	include( add_project )
	
else()
    message( "Python not found! Skipping project python!" )
endif()
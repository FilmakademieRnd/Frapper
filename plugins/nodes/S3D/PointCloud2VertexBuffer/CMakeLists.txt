project(pointCloud2VertexBuffer)

# pointcloud2vertexbuffer
set( res_header 		
	pointCloud2VertexBufferNode.h
	pointCloud2VertexBufferNodePlugin.h
	../CharonInterface/CharonInterface.h
	../CharonInterface/CharonInterface.hpp
	)

set( res_moc 	
	pointCloud2VertexBufferNode.h
	pointCloud2VertexBufferNodePlugin.h
	../CharonInterface/CharonInterface.h  
	)

set( res_source 	
	pointCloud2VertexBufferNode.cpp
	pointCloud2VertexBufferNodePlugin.cpp
	)

set( res_description 	
	pointcloud2vertexbuffer.xml
	)

FIND_PACKAGE(charon-core REQUIRED)
FIND_PACKAGE(charon-utils REQUIRED)

if( charon-core_FOUND AND charon-utils_FOUND )

	set ( add_include_dir
		${charon-core_INCLUDE_DIRS}
		${charon-utils_INCLUDE_DIRS}
		${CIMG_INCLUDE_DIRS}
	)
		
	# Add library dependencies
	set( add_link_lib 
		charon-core
		${CIMG_LINK_LIBRARIES}
		optimized frappergui debug frappergui_d
	)

	include( add_project )
else()
    message( "charon-core and/or charon-utils not found! Skipping project pointCloud2VertexBuffer!" )
endif()
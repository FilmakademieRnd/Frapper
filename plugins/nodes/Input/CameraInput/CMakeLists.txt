project(camerainput)

# camerainput
set( res_header
			CameraInputNode.h
			CameraInputNodePlugin.h
			VideoSource.h
			)

set( res_moc
			CameraInputNode.h
			CameraInputNodePlugin.h
			)

set( res_source
			CameraInputNode.cpp
			CameraInputNodePlugin.cpp
			VideoSource.cpp
			)

set( res_description
			CameraInputNode.xml
			)

# add additional include directories
set( add_include_dir
	${OpenCV_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/plugins/resources/opencvinput/
)
			
# Add additional library dependencies
set( add_link_lib
	${OpenCV_LIBS}
)

include( add_project )
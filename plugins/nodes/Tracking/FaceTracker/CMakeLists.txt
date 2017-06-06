project(facetracker)

# facetracker
set( res_header
			${CMAKE_SOURCE_DIR}/plugins/resources/opencvinput/opencvColorInput.h
			Face.h
			FaceTrackerNode.h
			FaceTrackerNodePlugin.h
			Helper.h
			)

set( res_moc
			${CMAKE_SOURCE_DIR}/plugins/resources/opencvinput/opencvColorInput.h
			FaceTrackerNode.h
			FaceTrackerNodePlugin.h
			)

set( res_source
			${CMAKE_SOURCE_DIR}/plugins/resources/opencvinput/opencvColorInput.cpp
			Face.cpp
			FaceTrackerNode.cpp
			FaceTrackerNodePlugin.cpp
			)

set( res_description
			facetracker.xml
			haarcascade_frontalface_alt.cascade
			)

# facetracker node requires opencv
FIND_PACKAGE( OpenCV REQUIRED HINTS ${THIRDPARTY_DIR}/opencv )

IF( OpenCV_FOUND )

	# add additional include directories
	set( add_include_dir
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_SOURCE_DIR}/plugins/resources/opencvinput/
	)
				
	# Add additional library dependencies
	set( add_link_lib
		${OpenCV_LIBS}
		optimized frappercore debug frappercore_d
	)

	include( add_project )
	
ENDIF()
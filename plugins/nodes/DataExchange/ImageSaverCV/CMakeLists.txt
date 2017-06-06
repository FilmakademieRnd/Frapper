project(imagesavercv)

# imagesaver
set( res_header
			ImageSaverCVNode.h
			ImageSaverCVNodePlugin.h
			)

set( res_moc
			ImageSaverCVNode.h
			ImageSaverCVNodePlugin.h
			)

set( res_source
			ImageSaverCVNode.cpp
			ImageSaverCVNodePlugin.cpp
			)

set( res_description
			imagesavercv.xml
			)
			
# declare usage of opencv lib
FIND_PACKAGE( OpenCV REQUIRED HINTS ${THIRDPARTY_DIR}/opencv)			

IF( OpenCV_FOUND )

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
	
ENDIF()

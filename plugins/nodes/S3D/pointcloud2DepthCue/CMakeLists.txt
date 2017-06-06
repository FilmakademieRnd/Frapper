project(pointcloud2DepthCue)

set( res_header
	pointcloud2DepthCue.h
	pointcloud2DepthCuePlugin.h
	)

set( res_moc
	pointcloud2DepthCue.h
	pointcloud2DepthCuePlugin.h
	)

set( res_source
	pointcloud2DepthCue.cpp
	pointcloud2DepthCuePlugin.cpp
	)

set( res_description
	pointcloud2DepthCue.xml
	)

# Check pointcloud2DepthCue node dependencies..
	#---- OpenCV -------
	FIND_PACKAGE(OpenCV REQUIRED HINTS ${THIRDPARTY_DIR}/opencv )

	IF( OpenCV_FOUND )
		message("CV Found")
		# add additional include directories
		set( add_include_dir
			${OpenCV_INCLUDE_DIRS}
		)

		# Add additional library dependencies
		set( add_link_lib
			${OpenCV_LIBS}
		)

		include( add_project )
	
	ENDIF() # OpenCV_FOUND
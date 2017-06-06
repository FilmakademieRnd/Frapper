project(smieyetracker)

# add additional include directories
set( add_include_dir
		${THIRDPARTY_DIR}/SMI_EyeTracker/include
	)
				
# Add additional library dependencies
set( add_link_dir
	${THIRDPARTY_DIR}/SMI_EyeTracker/lib
	)
	
# smieyetracker
set( res_header
			SMIEyeTrackerNode.h
			SMIEyeTrackerNodePlugin.h
			)

set( res_moc
			SMIEyeTrackerNode.h
			SMIEyeTrackerNodePlugin.h
			)

set( res_source
			SMIEyeTrackerNode.cpp
			SMIEyeTrackerNodePlugin.cpp
			)

set( res_description
			SMIEyeTracker.xml
			)

# Add library dependencies
set( add_link_lib 
	optimized QtMultimedia4 debug QtMultimediad4
	optimized QtNetwork4 debug QtNetworkd4
	optimized iViewXAPI debug iViewXAPI 
	optimized iViewXAPI64 debug iViewXAPI64 
	)
include( add_project )
	

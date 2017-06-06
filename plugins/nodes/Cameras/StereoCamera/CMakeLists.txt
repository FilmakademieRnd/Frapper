project(stereocamera)

# StereoCamera
set( res_header
			StereoCamera.h
			StereoCameraPlugin.h
			)

set( res_moc
			StereoCamera.h
			StereoCameraPlugin.h
			)

set( res_source
			StereoCamera.cpp
			StereoCameraPlugin.cpp
			)
			
set( res_description
			stereocamera.xml
			)

include( add_project )
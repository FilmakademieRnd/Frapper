project(camera)

# camera
set( res_header
			CameraNode.h
			CameraNodePlugin.h
			)

set( res_moc
			CameraNode.h
			CameraNodePlugin.h
			)

set( res_source
			CameraNode.cpp
			CameraNodePlugin.cpp
			)

set( res_description
			camera.xml
			)

include( add_project )
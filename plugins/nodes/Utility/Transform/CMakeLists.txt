project(transform)

# transform
set( res_header
			TransformNode.h
			TransformNodePlugin.h
			)

set( res_moc
			TransformNode.h
			TransformNodePlugin.h
			)

set( res_source
			TransformNode.cpp
			TransformNodePlugin.cpp
			)

set( res_description
			transform.xml
			)

include( add_project )
project(TextureGeometryMover)

# TextureGeometryMover
set( res_header			
			TextureGeometryMoverNode.h
			TextureGeometryMoverNodePlugin.h
			)

set( res_moc
			TextureGeometryMoverNode.h
			TextureGeometryMoverNodePlugin.h
			)

set( res_source
			TextureGeometryMoverNode.cpp
			TextureGeometryMoverNodePlugin.cpp
			)

set( res_description
			TextureGeometryMoverNode.xml
			)

include( add_project )
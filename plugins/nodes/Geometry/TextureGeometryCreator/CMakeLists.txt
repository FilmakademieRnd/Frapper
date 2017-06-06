project(TextureGeometryCreator)

# TextureGeometryCreator
set( res_header			
			TextureGeometryCreatorNode.h
			TextureGeometryCreatorNodePlugin.h
			)

set( res_moc
			TextureGeometryCreatorNode.h
			TextureGeometryCreatorNodePlugin.h
			)

set( res_source
			TextureGeometryCreatorNode.cpp
			TextureGeometryCreatorNodePlugin.cpp
			)

set( res_description
			TextureGeometryCreatorNode.xml
			)

include( add_project )
project(PositionBufferCreator)

# PositionBufferCreator
set( res_header			
			PositionBufferCreatorNode.h
			PositionBufferCreatorNodePlugin.h
			)

set( res_moc
			PositionBufferCreatorNode.h
			PositionBufferCreatorNodePlugin.h
			)

set( res_source
			PositionBufferCreatorNode.cpp
			PositionBufferCreatorNodePlugin.cpp
			)

set( res_description
			PositionBufferCreator.xml
			)

include( add_project )
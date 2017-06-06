project(PointCreator)

# PointCreator
set( res_header			
			PointCreatorNode.h
			PointCreatorNodePlugin.h
			)

set( res_moc
			PointCreatorNode.h
			PointCreatorNodePlugin.h
			)

set( res_source
			PointCreatorNode.cpp
			PointCreatorNodePlugin.cpp
			)

set( res_description
			PointCreatorNode.xml
			)

include( add_project )
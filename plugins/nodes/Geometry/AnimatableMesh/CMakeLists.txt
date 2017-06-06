project(animatablemesh)

# animatablemesh
set( res_header
			AnimatableMeshNode.h
			AnimatableMeshNodePlugin.h
			)

set( res_moc
			AnimatableMeshNode.h
			AnimatableMeshNodePlugin.h
			)

set( res_source
			AnimatableMeshNode.cpp
			AnimatableMeshNodePlugin.cpp
			)

set( res_description
			animatablemesh.xml
			)

include( add_project )
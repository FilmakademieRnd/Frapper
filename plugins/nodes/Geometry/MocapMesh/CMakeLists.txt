project(mocapmesh)

# mocapmesh
set( res_header			
			MocapMeshNode.h
			MocapMeshNodePlugin.h
			)

set( res_moc
			MocapMeshNode.h
			MocapMeshNodePlugin.h
			)

set( res_source
			MocapMeshNode.cpp
			MocapMeshNodePlugin.cpp
			)

set( res_description
			mocapmesh.xml
			)

include( add_project )
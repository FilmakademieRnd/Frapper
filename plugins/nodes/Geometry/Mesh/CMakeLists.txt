project(Mesh)

# animatablemesh
set( res_header
	MeshNode.h
	MeshNodePlugin.h
)

set( res_moc
	MeshNode.h
	MeshNodePlugin.h
)

set( res_source
	MeshNode.cpp
	MeshNodePlugin.cpp
)

set( res_description
	MeshNode.xml
)

include( add_project )
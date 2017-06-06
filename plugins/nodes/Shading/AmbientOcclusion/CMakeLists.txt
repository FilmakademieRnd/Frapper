project(ambientocclusion)

set( res_header
    AmbientOcclusionNode.h
    AmbientOcclusionNodePlugin.h
)

set( res_moc
    AmbientOcclusionNode.h
    AmbientOcclusionNodePlugin.h
)

set( res_source
    AmbientOcclusionNode.cpp
    AmbientOcclusionNodePlugin.cpp
)

set( res_description
	ambientocclusion.xml
)

include( add_project )
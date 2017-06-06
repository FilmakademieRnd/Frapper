project(multitexture)

# multitexture
set( res_header
    MultiTextureNode.h
    MultiTextureNodePlugin.h
    )

set( res_moc
    MultiTextureNode.h
    MultiTextureNodePlugin.h
    )

set( res_source
    MultiTextureNode.cpp
    MultiTextureNodePlugin.cpp
    )

set( res_description    
	multitexture.xml
)

include( add_project )
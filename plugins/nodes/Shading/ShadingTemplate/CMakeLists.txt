project( ShadingTemplate)

set( res_header
    ShadingTemplateNode.h
	ShadingTemplateNodePlugin.h
)

set( res_moc 
	ShadingTemplateNode.h
	ShadingTemplateNodePlugin.h
)

set( res_source
	ShadingTemplateNode.cpp
	ShadingTemplateNodePlugin.cpp
)

set( res_shader
    shader/ShadingTemplate.cg
    shader/ShadingTemplate.compositor
    shader/ShadingTemplate.material
)

include( add_project )
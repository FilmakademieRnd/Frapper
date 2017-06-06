project(CelShading)

set( res_description
	CelShading.xml
)

set( res_shader
    shader/CelShading.cg
    shader/CelShading.compositor
    shader/CelShading.material
    shader/cel_shading_diffuse.png
    shader/cel_shading_edge.png
    shader/cel_shading_specular.png
)

include( add_project)
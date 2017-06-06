project(ColorKey)

set( res_description
	colorkey.xml
)

set( res_shader
    shader/ColorKey.cg
    shader/ColorKey.compositor
    shader/ColorKey.material
)

include( add_project )
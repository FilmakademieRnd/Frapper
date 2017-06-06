project(BoxBlur)

set( res_description
	boxblur.xml
)

set( res_shader
    shader/BoxBlur.cg
    shader/BoxBlur.compositor
    shader/BoxBlur.material
)

include( add_project )
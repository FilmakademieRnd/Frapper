project(GaussBlur)

set( res_description
	gaussblur.xml
)

set( res_shader
    shader/GaussBlur.cg
    shader/GaussBlur.compositor
    shader/GaussBlur.material
)

include( add_project )
project(DepthOfFieldShading)

set( res_description
	depthoffield.xml
)

set( res_shader
    shader/DepthOfField.cg
    shader/DepthOfField.compositor
    shader/DepthOfField.material
)

include( add_project )
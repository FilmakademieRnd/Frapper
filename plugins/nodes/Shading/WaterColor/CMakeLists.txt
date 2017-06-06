project(WaterColor)

set( res_description
	WaterColorRes.xml
)

set( res_shader
    shader/AquarellBlur40.frag.glsl
    shader/AquarellBlur40.vert.glsl
    shader/AquarellCombination40.frag.glsl
    shader/AquarellCombination40.vert.glsl
    shader/AquarellID40.frag.glsl
    shader/AquarellID40.vert.glsl
    shader/AquarellRes40.frag.glsl
    shader/AquarellRes40.vert.glsl
    shader/AquarellTexture40.frag.glsl
    shader/AquarellTexture40.vert.glsl
    shader/Object40.frag.glsl
    shader/Object40.vert.glsl
    shader/Watercolor.cg
    shader/Watercolor.compositor
    shader/Watercolor.material
)

include( add_project )
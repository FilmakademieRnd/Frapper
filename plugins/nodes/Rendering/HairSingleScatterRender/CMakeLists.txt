project(hairsinglescatterrender)

# hairsinglescatterrender
set( res_header
			HairSingleScatterRenderNode.h
			HairSingleScatterRenderNodePlugin.h
			)

set( res_moc
			HairSingleScatterRenderNode.h
			HairSingleScatterRenderNodePlugin.h
			)

set( res_source
			HairSingleScatterRenderNode.cpp
			HairSingleScatterRenderNodePlugin.cpp
			)
			
set( res_description
			hairsinglescatterrender.xml
			)

include( add_project )
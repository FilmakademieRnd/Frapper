project(hairrender)

# hairrender
set( res_header
			DualScatterMath.h
			HairRenderNode.h
			HairRenderNodePlugin.h
			)

set( res_moc
			HairRenderNode.h
			HairRenderNodePlugin.h
			)

set( res_source
			DualScatterMath.cpp	
			HairRenderNode.cpp
			HairRenderNodePlugin.cpp
			)
			
set( res_description
			hairrender.xml
			)

include( add_project )
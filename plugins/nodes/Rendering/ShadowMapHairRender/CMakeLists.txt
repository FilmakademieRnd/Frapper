project(shadowmaphairrender)

# shadowmaphairrender
set( res_header
			ShadowMapHairRenderNode.h
			ShadowMapHairRenderNodePlugin.h
			)

set( res_moc
			ShadowMapHairRenderNode.h
			ShadowMapHairRenderNodePlugin.h
			)

set( res_source
			ShadowMapHairRenderNode.cpp
			ShadowMapHairRenderNodePlugin.cpp
			)
			
set( res_description
			shadowmaphairrender.xml
			)

include( add_project )
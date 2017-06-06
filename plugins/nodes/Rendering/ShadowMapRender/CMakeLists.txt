project(shadowmaprender)

# shadowmaprender
set( res_header
			ShadowMapRenderNode.h
			ShadowMapRenderNodePlugin.h
			)

set( res_moc
			ShadowMapRenderNode.h
			ShadowMapRenderNodePlugin.h
			)

set( res_source
			ShadowMapRenderNode.cpp
			ShadowMapRenderNodePlugin.cpp
			)
			
set( res_description
			shadowmaprender.xml
			)

include( add_project )
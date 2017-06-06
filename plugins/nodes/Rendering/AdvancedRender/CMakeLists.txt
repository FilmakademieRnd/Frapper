project(advancedrender)

# advancedrender
set( res_header
			AdvancedRenderNode.h
			AdvancedRenderNodePlugin.h
			)

set( res_moc
			AdvancedRenderNode.h
			AdvancedRenderNodePlugin.h
			)

set( res_source
			AdvancedRenderNode.cpp
			AdvancedRenderNodePlugin.cpp
			)
			
set( res_description
			advancedrender.xml
			)

include( add_project )
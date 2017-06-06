project(simplerender)

# simplerender
set( res_header
			SimpleRenderNode.h
			SimpleRenderNodePlugin.h
			)

set( res_moc
			SimpleRenderNode.h
			SimpleRenderNodePlugin.h
			)

set( res_source
			SimpleRenderNode.cpp
			SimpleRenderNodePlugin.cpp
			)
			
set( res_description
			simplerender.xml
			)

include( add_project )
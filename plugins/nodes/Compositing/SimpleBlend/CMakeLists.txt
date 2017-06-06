project(simpleblend)

# simpleblend
set( res_header
			SimpleBlendNode.h
			SimpleBlendNodePlugin.h
			)

set( res_moc
			SimpleBlendNode.h
			SimpleBlendNodePlugin.h
			)

set( res_source
			SimpleBlendNode.cpp
			SimpleBlendNodePlugin.cpp
			)

set( res_description
			simpleblend.xml
			)

include( add_project )
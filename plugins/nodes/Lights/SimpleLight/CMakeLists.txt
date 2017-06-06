project(simplelight)

# light
set( res_header
			SimpleLightNode.h
			SimpleLightNodePlugin.h
			)

set( res_moc
			SimpleLightNode.h
			SimpleLightNodePlugin.h
			)

set( res_source
			SimpleLightNode.cpp
			SimpleLightNodePlugin.cpp
			)

set( res_description
			simplelight.xml
			)

include( add_project )
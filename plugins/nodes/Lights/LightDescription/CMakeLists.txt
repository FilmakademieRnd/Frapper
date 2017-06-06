project(lightdescription)

# lightdescription
set( res_header
			LightDescriptionNode.h
			LightDescriptionNodePlugin.h
			)

set( res_moc
			LightDescriptionNode.h
			LightDescriptionNodePlugin.h
			)

set( res_source
			LightDescriptionNode.cpp
			LightDescriptionNodePlugin.cpp
			)

set( res_description
			lightdescription.xml
			)

include( add_project )
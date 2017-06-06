project(clouddisplay)

# clouddisplay
set( res_header
			CloudDisplayNode.h
			CloudDisplayNodePlugin.h
			TagCloudView.h
			)

set( res_moc
			CloudDisplayNode.h
			CloudDisplayNodePlugin.h
			TagCloudView.h
			)

set( res_source
			CloudDisplayNode.cpp
			CloudDisplayNodePlugin.cpp
			TagCloudView.cpp
			)

set( res_description
			clouddisplay.xml
			)

set( add_link_lib
	optimized frappergui debug frappergui_d
)

include( add_project )
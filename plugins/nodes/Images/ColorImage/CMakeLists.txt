project(colorimage)

# colorimage
set( res_header
			ColorImageNode.h
			ColorImageNodePlugin.h
			)

set( res_moc
			ColorImageNode.h
			ColorImageNodePlugin.h
			)

set( res_source
			ColorImageNode.cpp
			ColorImageNodePlugin.cpp
			)

set( res_description
			colorimage.xml
			)

include( add_project )
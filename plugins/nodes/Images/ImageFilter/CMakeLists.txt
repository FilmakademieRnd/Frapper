project(imagefilter)

# imagefilter
set( res_header
			ImageFilterNode.h
			ImageFilterNodePlugin.h
			)

set( res_moc
			ImageFilterNode.h
			ImageFilterNodePlugin.h
			)

set( res_source
			ImageFilterNode.cpp
			ImageFilterNodePlugin.cpp
			)

set( res_description
			imagefilter.xml
			)

include( add_project )
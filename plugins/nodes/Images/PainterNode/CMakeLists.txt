project(painternode)

# painer
set( res_header
			PainterNode.h
			PainterNodePlugin.h
			)

set( res_moc
			PainterNode.h
			PainterNodePlugin.h
			)

set( res_source
			PainterNode.cpp
			PainterNodePlugin.cpp
			)
			
set( res_description
			painternode.xml
			)

# add painter panel includes / libs
get_property( painterpanel_INCLUDE_DIRS GLOBAL PROPERTY painterpanel_INCLUDE_DIRS )
get_property( painterpanel_LIBRARIES GLOBAL PROPERTY painterpanel_LIBRARIES )

set ( add_include_dir ${painterpanel_INCLUDE_DIRS} )
set ( add_link_lib ${painterpanel_LIBRARIES} )

include( add_project )
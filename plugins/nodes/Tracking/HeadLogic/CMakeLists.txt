project(headlogic)

# headlogic
set( res_header
			HeadLogicNode.h
			HeadLogicNodePlugin.h
			)

set( res_moc
			HeadLogicNode.h
			HeadLogicNodePlugin.h
			)

set( res_source
			HeadLogicNode.cpp
			HeadLogicNodePlugin.cpp
			)

set( res_description
			headlogic.xml
			)

include( add_project )
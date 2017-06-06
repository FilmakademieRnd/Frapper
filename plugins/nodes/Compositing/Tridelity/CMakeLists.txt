project(tridelity)

# tridelity
set( res_header
			TridelityNode.h
			TridelityNodePlugin.h
			)

set( res_moc
			TridelityNode.h
			TridelityNodePlugin.h
			)

set( res_source
			TridelityNode.cpp
			TridelityNodePlugin.cpp
			)

set( res_description
			tridelity.xml
			)

include( add_project )
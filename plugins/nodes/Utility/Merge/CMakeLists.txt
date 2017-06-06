project(merge)

# merge
set( res_header
			mergeNode.h
			mergeNodePlugin.h
			)

set( res_moc
			mergeNode.h
			mergeNodePlugin.h
			)

set( res_source
			mergeNode.cpp
			mergeNodePlugin.cpp
			)

set( res_description
			merge.xml
			)

include( add_project )
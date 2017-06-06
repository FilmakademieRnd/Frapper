project(split)

# split
set( res_header
			splitNode.h
			splitNodePlugin.h
			)

set( res_moc
			splitNode.h
			splitNodePlugin.h
			)

set( res_source
			splitNode.cpp
			splitNodePlugin.cpp
			)

set( res_description
			split.xml
			)

include( add_project )
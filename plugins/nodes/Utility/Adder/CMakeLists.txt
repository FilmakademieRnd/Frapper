project(adder)

# adder
set( res_header
			AdderNode.h
			AdderNodePlugin.h
			)

set( res_moc
			AdderNode.h
			AdderNodePlugin.h
			)

set( res_source
			AdderNode.cpp
			AdderNodePlugin.cpp
			)

set( res_description
			adder.xml
			)

include( add_project )
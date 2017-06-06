project(testNode)

# test
set( res_header
			testNode.h
			testNodePlugin.h
			)

set( res_moc
			testNode.h
			testNodePlugin.h
			)

set( res_source
			testNode.cpp
			testNodePlugin.cpp
			)

set( res_description
			test.xml
			)

include( add_project )
project(mean)

# mean
set( res_header
			meanNode.h
			meanNodePlugin.h
			)

set( res_moc
			meanNode.h
			meanNodePlugin.h
			)

set( res_source
			meanNode.cpp
			meanNodePlugin.cpp
			)

set( res_description
			mean.xml
			)

include( add_project )
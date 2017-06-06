project(Index2Index)

# Index2Index
set( res_header
			Index2IndexNode.h
			Index2IndexNodePlugin.h
			)

set( res_moc
			Index2IndexNode.h
			Index2IndexNodePlugin.h
			)

set( res_source
			Index2IndexNode.cpp
			Index2IndexNodePlugin.cpp
			)

set( res_description
			index2index.xml
			)

include( add_project )
project(Index2Viseme)

# Index2Viseme
set( res_header
			Index2VisemeNode.h
			Index2VisemeNodePlugin.h
			)

set( res_moc
			Index2VisemeNode.h
			Index2VisemeNodePlugin.h
			)

set( res_source
			Index2VisemeNode.cpp
			Index2VisemeNodePlugin.cpp
			)

set( res_description
			index2viseme.xml
			)

include( add_project )
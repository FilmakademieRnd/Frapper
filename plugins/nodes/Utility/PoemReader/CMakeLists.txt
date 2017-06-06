project(poemreader)

# poemreader
set( res_header
			PoemReaderNode.h
			PoemReaderNodePlugin.h
			)

set( res_moc
			PoemReaderNode.h
			PoemReaderNodePlugin.h
			)
set( res_source

			PoemReaderNode.cpp
			PoemReaderNodePlugin.cpp
			)

set( res_description
			poemreader.xml
			)

set( add_link_lib
    optimized frappergui debug frappergui_d
)
            
include( add_project )
project(stringmultiplexer)

# stringmultiplexer
set( res_header
			StringMultiplexerNode.h
			StringMultiplexerNodePlugin.h
			)

set( res_moc
			StringMultiplexerNode.h
			StringMultiplexerNodePlugin.h
			)

set( res_source
			StringMultiplexerNode.cpp
			StringMultiplexerNodePlugin.cpp
			)

set( res_description
			stringmultiplexer.xml
			)

include( add_project )
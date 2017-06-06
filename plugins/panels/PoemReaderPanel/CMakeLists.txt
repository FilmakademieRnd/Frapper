project(poemreaderpanel)

set ( add_include_dir
	${CMAKE_SOURCE_DIR}/plugins/nodes/Agent/PoemReader
)

# poemreaderpanel
set( res_header 
	poemreaderpanel.h
	poemreaderpanelPlugin.h
)

set( res_moc 	
	poemreaderpanel.h
	poemreaderpanelPlugin.h
)
			
set( res_source 	
	poemreaderpanel.cpp
	poemreaderpanelPlugin.cpp
)

set( res_description 	
	poemreaderpanel.xml
)

# Add library dependencies
set( add_link_lib
	optimized frappergui debug frappergui_d
)

include( add_project )
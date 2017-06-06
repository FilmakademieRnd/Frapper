project(universalGraph)

set ( add_include_dir
	${CMAKE_CURRENT_SOURCE_DIR}
	)

# universalGraph
set( res_header 
	universalGraph.h
	universalGraphPlugin.h
	)

set( res_moc 	
	universalGraph.h
	universalGraphPlugin.h
)
		
set( res_source 	
	universalGraph.cpp
	universalGraphPlugin.cpp
	)

set( res_description 	
	universalgraph.xml
	)

# Add library dependencies
set( add_link_lib
	optimized frappergui debug frappergui_d
)

include( add_project )
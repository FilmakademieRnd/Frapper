project(testpanel)

# testpanel
set( res_header 
	testpanel.h
	testpanelPlugin.h
	)

set( res_moc 	
	testpanel.h
	testpanelPlugin.h
	)
			
set( res_source 	
	testpanel.cpp
	testpanelPlugin.cpp
	)

set( res_description 	
	testpanel.xml
	)


# Add library dependencies
set( add_link_lib
	optimized frappergui debug frappergui_d
)

include( add_project )
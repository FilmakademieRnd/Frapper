project(frapperdemo)

set( res_header
	Application.h
)

set( res_moc
	Application.h
)

set( res_source
	Application.cpp
	main.cpp
)
		   
set ( res_qrc
	../../resources/main.qrc
) 

set( res_additional
	styles/default.css
	frapper.rc
	frapper.ico
)

# Create as executable
set( create_executable TRUE)

if( WIN32 )
	set( add_link_lib 
		optimized ${COLLADA_DOM_LIB} debug ${COLLADA_DOM_LIB_DEBUG}
		optimized frappergui debug frappergui_d
	)
elseif( UNIX) 
	set( add_link_lib 
		optimized minizip debug minizip
		optimized tools debug tools_d
		optimized frappercore debug frappercore_d
		optimized frappergui debug frappergui_d
		optimized collada14dom debug collada14dom
	)
endif()

include( add_project )

# add define for hiding console
IF( FRAPPER_HIDE_CONSOLE_IN_RELEASE )
	SET_TARGET_PROPERTIES (frapperdemo PROPERTIES
		COMPILE_DEFINITIONS_RELEASE HIDE_CONSOLE
		COMPILE_DEFINITIONS_RELWITHDEBINFO HIDE_CONSOLE
		COMPILE_DEFINITIONS_MINSIZEREL HIDE_CONSOLE
	)
ENDIF( FRAPPER_HIDE_CONSOLE_IN_RELEASE )

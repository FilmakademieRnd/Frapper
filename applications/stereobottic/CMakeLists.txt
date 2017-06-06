project(StereoBottic)

set( res_header
	StereoBottic.h
)

set( res_moc
	StereoBottic.h
)

set( res_source
    StereoBottic.cpp
    main.cpp
)

set( res_qrc
	StereoBottic.qrc
)
		   
# add icon to exe
set ( res_additional
    StereoBottic.rc
	styles/stereobottic.css
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
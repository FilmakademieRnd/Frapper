project(frapperogreconfig)

set( res_header
	FrapperOgreConfig.h
)

set( res_source
	FrapperOgreConfig.cpp
)
		   
set( res_additional
	FrapperOgreConfig.rc
	FrapperOgreConfig.ico
)		   

set( create_executable TRUE)
		   
include( add_project )

# set subsystem to windows to supress console
if( WIN32 )
	set_target_properties( frapperogreconfig PROPERTIES WIN32_EXECUTABLE ON)
endif()
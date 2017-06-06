project(charonworkflow)

# charon
set( res_header 		
    CharonWorkflow.h
    CharonWorkflowNode.h
    CharonWorkflowPlugin.h
    )

set( res_moc
    CharonWorkflow.h
    CharonWorkflowNode.h
    CharonWorkflowPlugin.h
    )

set( res_source 	
    CharonWorkflow.cpp
    CharonWorkflowNode.cpp
    CharonWorkflowPlugin.cpp
    )

set( res_description
	charonworkflow.xml
)

# charon node requires charon-core
FIND_PACKAGE( charon-core REQUIRED)
if( charon-core_FOUND )
	set( add_include_dir ${charon-core_INCLUDE_DIRS} )
	set( add_link_lib 
		optimized frappergui debug frappergui_d
		charon-core
		charon-plugins
		)
	include( add_project )
else()
    message( "charon-core not found! Skipping project charonworkflow!" )
endif()


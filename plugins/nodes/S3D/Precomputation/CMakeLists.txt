project(precomputation)

# precomputation
set( res_header 		
    PreComputationNode.h
    PreComputationPlugin.h
    )

set( res_moc
    PreComputationNode.h
    PreComputationPlugin.h
    )

set( res_source 	
    PreComputationNode.cpp
    PreComputationPlugin.cpp
    )

set( res_description
	precomputation.xml
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
    message( "charon-core not found! Skipping project precomputation!" )
endif()


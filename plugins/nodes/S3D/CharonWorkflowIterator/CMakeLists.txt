project(CharonWorkflowIterator)

# charon
set( res_header
    CharonWorkflowIteratorNode.h
    CharonWorkflowIteratorPlugin.h
)

set( res_moc
    CharonWorkflowIteratorNode.h
    CharonWorkflowIteratorPlugin.h
)

set( res_source
    CharonWorkflowIteratorNode.cpp
    CharonWorkflowIteratorPlugin.cpp
)

set( res_description
    charonworkflowiterator.xml
)

# charon node requires charon-core
FIND_PACKAGE( charon-core REQUIRED)
if( charon-core_FOUND )
  set( add_include_dir ${charon-core_INCLUDE_DIRS})
  set( add_link_lib 
    optimized frappergui debug frappergui_d
    charon-core charon-plugins
  )
  include( add_project )
else()
    message( "charon-core not found! Skipping project CharonWorkflowIterator!" )
endif()

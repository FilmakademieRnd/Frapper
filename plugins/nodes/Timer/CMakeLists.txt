project(Timer)

if (FRAPPER_BUILD_NODES_TIMER OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory Timer")
  add_subdirectory( Timer )
  add_subdirectory( MidiTimer )
else()  
  message( STATUS "Skipping plugins from directory Timer")
endif()



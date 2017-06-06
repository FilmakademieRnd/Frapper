project(Network)

if (FRAPPER_BUILD_NODES_NETWORK OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory Network")
  add_subdirectory( Broadcast )
  add_subdirectory( FaceShiftClient )
  add_subdirectory( AvateeringClient )
  add_subdirectory( FaceShiftClientUDP )
  add_subdirectory( NetworkAnim )
else()  
  message( STATUS "Skipping plugins from directory Network")
endif()


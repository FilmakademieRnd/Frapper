project(Tracking)

if (FRAPPER_BUILD_NODES_TRACKING OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory Tracking")
  add_subdirectory( FaceTracker )
  add_subdirectory( HeadLogic )
else()  
  message( STATUS "Skipping plugins from directory Tracking")
endif()


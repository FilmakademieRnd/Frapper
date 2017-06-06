project(Images)

if (FRAPPER_BUILD_NODES_IMAGES OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory Images")
  add_subdirectory( ColorImage )
  add_subdirectory( ImageFile )
  add_subdirectory( ImageFilter )
  add_subdirectory( CloudDisplay )
  add_subdirectory( PainterNode ) 
else()  
  message( STATUS "Skipping plugins from directory Images")
endif()



project(Lights)

if (FRAPPER_BUILD_NODES_LIGHTS OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory Light")
  add_subdirectory(SimpleLight)
  add_subdirectory(LightDescription)
else()  
  message( STATUS "Skipping plugins from directory Light")
endif()

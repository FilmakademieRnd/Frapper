project(NPR)

if (FRAPPER_BUILD_NODES_NPR OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory NPR")
  add_subdirectory( FilterBilateralSmooth )
  add_subdirectory( FilterMorphologicalSmooth )
  add_subdirectory( FilterSobel )
  add_subdirectory( FilterUnsharpMask )
  add_subdirectory( LooseAndSketchyOffline )
  add_subdirectory( LooseAndSketchy2 )
  #add_subdirectory( LooseAndSketchy )
  add_subdirectory( Watercolor )
else()  
  message( STATUS "Skipping plugins from directory NPR")
endif()



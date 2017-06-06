project(DataExchange)

if (FRAPPER_BUILD_NODES_DATAEXCHANGE)
  message( STATUS "Adding plugins from directory Export")
  add_subdirectory( Alembic )
  add_subdirectory( ImageSaver )
  add_subdirectory( ImageSaverCV )
  add_subdirectory( ImageSaverOgre )
else()  
  message( STATUS "Skipping plugins from directory Export")
endif()


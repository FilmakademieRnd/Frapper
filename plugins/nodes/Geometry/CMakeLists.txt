project(Geometry)

if (FRAPPER_BUILD_NODES_GEOMETRY OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory Geometry")
  add_subdirectory( AnimatableMesh )
  add_subdirectory( AnimatableMeshHair )
  add_subdirectory( AnimatableMeshDECT )
  add_subdirectory( Mesh )
  add_subdirectory( MocapMesh )
  add_subdirectory( ParticleSystem ) 
  add_subdirectory( PointCloudViewer )
  add_subdirectory( SceneLoader )
  #add_subdirectory( PointCreator )
  #add_subdirectory( PointMover )
	
else()  
	message( STATUS "Skipping plugins from directory Geometry")
endif()


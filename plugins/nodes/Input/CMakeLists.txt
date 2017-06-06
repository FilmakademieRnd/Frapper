project(Input)

IF (FRAPPER_BUILD_NODES_INPUT OR FRAPPER_BUILD_DEMO)
  message( STATUS "Adding plugins from directory Input")

	IF (IS_DIRECTORY ${THIRDPARTY_DIR}/bass)
	  add_subdirectory( AudioPlayer)
	ELSE ()
	  message( WARNING "Bass SDK not found. The Audioplayer node will not be added.")
	ENDIF ()

	FIND_PACKAGE( OpenCV HINTS ${THIRDPARTY_DIR}/opencv)			
	IF( OpenCV_FOUND )
		add_subdirectory( CameraInput )
	ELSE()
		message( WARNING "OpenCV SDK not found. The CameraInput node will not be added.")
	ENDIF ()
	
	#add_subdirectory( WiiMote )
	add_subdirectory( KeyBoard )
	  
	IF (IS_DIRECTORY ${THIRDPARTY_DIR}/sixenseSDK)
		message( STATUS "Sixense SDK found.")
		add_subdirectory( RazerHydra )
	ELSE()
		message( WARNING "Sixense SDK not found. The RazerHydra node will not be added.")
	ENDIF()
	
	IF (IS_DIRECTORY ${THIRDPARTY_DIR}/SMI_EyeTracker)
		message( STATUS "SMI Eye Tracker SDK found.")
		add_subdirectory( SMIEyeTracker )
	ELSE()
		message( WARNING "SMI Eye Tracker SDK not found. The SMIEyeTracker node will not be added.")
	ENDIF()
  
ELSE()  
  message( STATUS "Skipping plugins from directory Input")
ENDIF()


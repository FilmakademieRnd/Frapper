project(frapper)

set( VS_PROJECT_FOLDER "Applications" )
message( STATUS "Adding projects from directory Applications")

add_subdirectory(frapperdemo)
add_subdirectory(frapperogreconfig)

if ( FRAPPER_BUILD_APPLICATIONS_STEREOBOTTIC)

    # check if StereoBottic assets are available
    if( EXISTS "${CMAKE_INSTALL_PREFIX}/scenes/StereoBottic/StereoBottic.dae")
        
		# S3D Game Node is required by Stereobottic
		set( FRAPPER_BUILD_NODES_S3DGAME ON)
		add_subdirectory(stereobottic)
    else()
        message( WARNING "StereoBottig game assets not found! Please download and install them from http://sourceforge.net/projects/frapper/files/StereoBottic/")
    endif()
else()
    message( STATUS "Skipping application Stereobottic")
endif()


project(StereoView)

# picviewershutter
set( res_header
    StereoViewNode.h
    StereoViewPlugin.h
	StereoViewWindow.h
	StereoViewD3D9Viewport.h
	#StereoViewNVApiWrapper.h
    StereoImageHeader.h
)

set( res_moc
    StereoViewNode.h
    StereoViewPlugin.h
	StereoViewWindow.h
)

set( res_source
    StereoViewNode.cpp
    StereoViewPlugin.cpp
	StereoViewWindow.cpp
	StereoViewD3D9Viewport.cpp
	#StereoViewNVApiWrapper.cpp
)

set( res_description
	stereoview.xml
)

set( res_additional
	images/3DVision.jpg
	images/StereoTest_1920x1080_L.jpg
	images/StereoTest_1920x1080_R.jpg
)

set( res_shader
    shader/StereoViewNode.cg
    shader/StereoViewNode.compositor
    shader/StereoViewNode.material
)

# add DirectX to project, uses findDirectX.cmake provided by Ogre
find_package(DirectX REQUIRED QUIET)
if( DirectX_FOUND )
	set( add_include_dir ${DirectX_INCLUDE_DIR} )
    set( add_link_lib ${DirectX_LIBRARIES} )

	# if( OGRE_RenderSystem_Direct3D9_FOUND )
		# list( APPEND add_include_dir ${OGRE_RenderSystem_Direct3D9_INCLUDE_DIRS} )
		# list( APPEND add_link_lib ${OGRE_RenderSystem_Direct3D9_LIBRARIES})
	# endif()
	
    include( add_project )
else()
    message( "DirectX not found! Skipping project StereoView!" )
endif()



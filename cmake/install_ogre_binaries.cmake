 
# add OGRE Dlls to package
if ( OGRE_FOUND AND FRAPPER_INSTALL_OGRE )

	# OgreMail.dll
	if ( OGRE_BINARY_REL )
        install( FILES ${OGRE_BINARY_REL} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ThirdParty )
    endif ()

	# OgreOverlay.dll
	if ( OGRE_Overlay_BINARY_REL )
        install( FILES ${OGRE_Overlay_BINARY_REL} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ThirdParty )
    endif ()
	
	# Plugin_CgProgramManager.dll
	if ( OGRE_Plugin_CgProgramManager_REL )
        install( FILES ${OGRE_Plugin_CgProgramManager_REL} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ThirdParty )
    endif ()

	# RenderSystem_GL.dll
	if ( OGRE_RenderSystem_GL_REL )
        install( FILES ${OGRE_RenderSystem_Direct3D11_REL} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ThirdParty )
    endif ()

	# RenderSystem_Direct3D9.dll
	if ( OGRE_RenderSystem_Direct3D9_REL )
        install( FILES ${OGRE_RenderSystem_Direct3D11_REL} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ThirdParty )
    endif ()

	# RenderSystem_Direct3D11.dll
	if ( OGRE_RenderSystem_Direct3D11_REL )
        install( FILES ${OGRE_RenderSystem_Direct3D11_REL} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ThirdParty )
    endif ()
endif ( OGRE_FOUND )

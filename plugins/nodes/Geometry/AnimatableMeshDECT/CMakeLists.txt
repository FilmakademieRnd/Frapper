project(animatablemeshdect)

# animatablemesh
set( res_header
			AnimatableMeshDECTNode.h
			AnimatableMeshDECTNodePlugin.h
			ObjectTextDisplay.h
			)

set( res_moc
			AnimatableMeshDECTNode.h
			AnimatableMeshDECTNodePlugin.h
			ObjectTextDisplay.h
			)

set( res_source
			AnimatableMeshDECTNode.cpp
			AnimatableMeshDECTNodePlugin.cpp
			ObjectTextDisplay.cpp
			)

set( res_description
			animatablemeshdect.xml
			)

# Add library dependencies
set( add_link_lib
	${OGRE_Overlay_LIBRARIES}
	${OGRE_RTShaderSystem_LIBRARIES} 
)

#add_definitions( -DOGRE_RTShader=${OGRE_DIR} )

include( add_project )
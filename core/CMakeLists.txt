project(frappercore)

set( res_header
	AbstractParameter.h
	GeometryAnimationNode.h
	BaseRectItem.h
	BackDropNode.h
	CompositorNode.h
	Connection.h
	ConnectionGraphicsItem.h
	CurveEditorDataNode.h
	EnumerationParameter.h
	FilenameParameter.h
	FlagGraphicsItem.h
	FrapperPlatform.h
	FrapperPrerequisites.h
	GenericParameter.h
	GeometryNode.h
	GeometryRenderNode.h
	Helper.h
	ImageNode.h
	InstanceCounterMacros.h
	Key.h
	LightNode.h
	Log.h
	ManualMesh.h
	MotionDataNode.h
	Node.h
	NodeFactory.h
	NodeGraphicsItem.h
	NodeBackDropGraphicsItem.h
	NodeGroupGraphicsItem.h
	NodeModel.h
	NodeType.h
	NodeTypeInterface.h
	NumberParameter.h
	OgreContainer.h
	OgreManager.h
	OgreTools.h
	Parameter.h
	ParameterAction.h
	ParameterGroup.h
	ParameterPlugin.h
	ParameterTypeIcon.h
	PinGraphicsItem.h
	TextureGeometryNode.h
	TextureGeometryNodeAbstract.h
	TextureGeometryShaderNode.h
	RenderNode.h
	SceneModel.h
	ViewFlagGraphicsItem.h
	ViewingParameters.h
	ViewNode.h
)

set( res_moc
	 AbstractParameter.h
	 BaseRectItem.h
	 BackDropNode.h
	 GeometryAnimationNode.h
	 CompositorNode.h
	 Connection.h
	 ConnectionGraphicsItem.h
	 CurveEditorDataNode.h
	 FlagGraphicsItem.h
	 GeometryNode.h
	 GeometryRenderNode.h
	 ImageNode.h
	 LightNode.h
	 MotionDataNode.h
	 Node.h	
	 NodeGraphicsItem.h
	 NodeBackDropGraphicsItem.h
	 NodeGroupGraphicsItem.h
	 NodeModel.h
	 NodeTypeInterface.h
	 OgreContainer.h
	 Parameter.h
	 ParameterGroup.h
	 NumberParameter.h
	 ParameterPlugin.h
	 PinGraphicsItem.h
	 TextureGeometryNode.h
	 TextureGeometryNodeAbstract.h
	 TextureGeometryShaderNode.h
	 RenderNode.h
	 SceneModel.h
	 SceneNodeParameter.h
	 ViewFlagGraphicsItem.h
	 ViewNode.h
)

set( res_source
	AbstractParameter.cpp	
	GeometryAnimationNode.cpp			
	BaseRectItem.cpp
	BackDropNode.cpp
	CompositorNode.cpp
	Connection.cpp
	ConnectionGraphicsItem.cpp
	CurveEditorDataNode.cpp
	EnumerationParameter.cpp
	FilenameParameter.cpp
	FlagGraphicsItem.cpp
	GenericParameter.cpp
	GeometryNode.cpp
	GeometryRenderNode.cpp
	ImageNode.cpp
	LightNode.cpp
	Log.cpp
	ManualMesh.cpp
	MotionDataNode.cpp
	Node.cpp
	NodeFactory.cpp
	NodeGraphicsItem.cpp
	NodeGroupGraphicsItem.cpp
	NodeBackDropGraphicsItem.cpp
	NodeModel.cpp
	NodeType.cpp
	NumberParameter.cpp
	OgreContainer.cpp
	OgreManager.cpp
	OgreTools.cpp
	Parameter.cpp
	ParameterAction.cpp
	ParameterGroup.cpp
	ParameterPlugin.cpp
	ParameterTypeIcon.cpp
	PinGraphicsItem.cpp
	TextureGeometryNode.cpp
	TextureGeometryNodeAbstract.cpp
	TextureGeometryShaderNode.cpp
	RenderNode.cpp
	SceneModel.cpp
	SceneNodeParameter.cpp
	ViewFlagGraphicsItem.cpp
	ViewingParameters.cpp
	ViewNode.cpp
)

set( res_description
)

set( add_link_lib
	${OGRE_Overlay_LIBRARIES}
	optimized ${COLLADA_DOM_LIB} debug ${COLLADA_DOM_LIB_DEBUG}
)

if( FRAPPER_LOG_EVALUATION )
	add_definitions( -DPARAMETER_EVALUATION_LOG)
endif()

# override install path
set( NODE_PLUGIN_INSTALL ${CMAKE_INSTALL_PREFIX})

# mark as core project to avoid self including
set( iscore TRUE)

include( add_project )
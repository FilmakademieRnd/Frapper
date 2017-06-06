project(frappergui)

set( res_header
   BaseGraphicsView.h
   Controller.h
   CopyHandler.h
   CurveEditorGraphicsView.h
   CurveEditorPanel.h
   DocumentationPanel.h
   DoubleSlider.h
   GrabberWidget.h
   GraphicsView.h
   HierarchyPanel.h
   HistoryPanel.h
   KeyGraphicsItem.h
   LogPanel.h
   LogProgressDialog.h
   PreCalcProgressDialog.h
   NetworkGraphicsView.h
   NetworkPanel.h
   Panel.h
   PanelFactory.h
   PanelFrame.h
   ParameterPanel.h
   ParameterTabPage.h
   PanelTypeInterface.h
   SegmentGraphicsItem.h
   TimelineGraphicsItem.h
   TimelinePanel.h
   TimelineWidget.h
   ViewPanel.h
   ViewportPanel.h
   ViewportWidget.h
   WidgetFactory.h
   WidgetPlugin.h
   Window.h
   WindowAction.h
   WindowItem.h
   WindowManager.h
   WindowsDialog.h
)

set( res_source
   BaseGraphicsView.cpp
   Controller.cpp
   CurveEditorGraphicsView.cpp
   CurveEditorPanel.cpp
   DocumentationPanel.cpp
   DoubleSlider.cpp
   GrabberWidget.cpp
   GraphicsView.cpp
   HierarchyPanel.cpp
   HistoryPanel.cpp
   KeyGraphicsItem.cpp
   LogPanel.cpp
   LogProgressDialog.cpp
   PreCalcProgressDialog.cpp
   NetworkGraphicsView.cpp
   NetworkPanel.cpp
   Panel.cpp
   PanelFactory.cpp
   PanelFrame.cpp
   ParameterPanel.cpp
   ParameterTabPage.cpp
   TimelinePanel.cpp
   TimelineWidget.cpp
   ViewPanel.cpp
   ViewportPanel.cpp
   ViewportWidget.cpp
   WidgetPlugin.cpp
   WidgetFactory.cpp
   Window.cpp
   WindowAction.cpp
   WindowItem.cpp
   WindowManager.cpp
   WindowsDialog.cpp
)

set( res_moc
	BaseGraphicsView.h
	CopyHandler.h
	CurveEditorGraphicsView.h
	CurveEditorPanel.h
	DocumentationPanel.h
	DoubleSlider.h
	GrabberWidget.h
	GraphicsView.h
	HierarchyPanel.h
	HistoryPanel.h
	LogPanel.h
	NetworkGraphicsView.h
	NetworkPanel.h
	Panel.h
	PanelFrame.h
	ParameterPanel.h
	ParameterTabPage.h
	PanelTypeInterface.h
	TimelinePanel.h
	TimelineWidget.h
	ViewPanel.h
	ViewportPanel.h
	ViewportWidget.h
	WidgetPlugin.h
	WidgetFactory.h
	WidgetTypeInterface.h
	Window.h
	WindowAction.h
	WindowsDialog.h
)

set( res_ui
   CurveEditorPanel.ui
   DocumentationPanel.ui	
   HierarchyPanel.ui
   HistoryPanel.ui
   LogPanel.ui
   LogProgressDialog.ui
   PreCalcProgressDialog.ui
   NetworkPanel.ui
   PanelFrame.ui
   ParameterPanel.ui
   TimelinePanel.ui
   ViewportPanel.ui
   Window.ui
   WindowsDialog.ui
)
		   
set ( res_qrc ../resources/gui.qrc )		   
		   
# Add library dependencies
set( add_link_lib
	${OGRE_Overlay_LIBRARIES}
)

# override install path
set( NODE_PLUGIN_INSTALL ${CMAKE_INSTALL_PREFIX})

include( add_project )

# include Tridelity stereo support
if (FRAPPER_BUILD_TRISTEREO)
	set_property(TARGET frappergui PROPERTY COMPILE_DEFINITIONS USE_STEREO)
endif (FRAPPER_BUILD_TRISTEREO)

project(painterpanel)

# painterpanel
set( res_header
	PainterPanel.h
	PainterPanelNode.h
	PainterPanelPlugin.h
	PainterGraphicsItems.h
	PainterGraphicsScene.h
	PainterGraphicsView.h
	PainterPanelItemData.h
	PainterPanelExports.h
	PainterUndoCommands.h
)

set( res_moc
	PainterPanel.h
	PainterPanelNode.h
	PainterPanelPlugin.h
	PainterGraphicsItems.h
	PainterGraphicsScene.h
	PainterGraphicsView.h
	PainterPanelItemData.h
)
			
set( res_source
	PainterPanel.cpp
	PainterPanelNode.cpp
	PainterPanelPlugin.cpp
	PainterGraphicsItems.cpp
	PainterGraphicsScene.cpp
	PainterGraphicsView.cpp
	PainterPanelItemData.cpp
	PainterUndoCommands.cpp
)

set( res_ui
	PainterPanelMainArea.ui
	PainterPanelToolBar.ui
)

set( res_description
	painterpanel.xml
)


# Add library dependencies
set( add_link_lib
	optimized frappergui debug frappergui_d
)

set( ispanel TRUE )
include( add_project )
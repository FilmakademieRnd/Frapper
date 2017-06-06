project(looseandsketchyoffline)

# looseandsketchyoffline
set( res_header 		
	LooseAndSketchyOfflineNode.h
	LooseAndSketchyOfflineNodePlugin.h
	Vector2d.h
	)

set( res_moc 	
	LooseAndSketchyOfflineNode.h
	LooseAndSketchyOfflineNodePlugin.h
	)

set( res_source 	
	LooseAndSketchyOfflineNode.cpp
	LooseAndSketchyOfflineNodePlugin.cpp
	)

set( res_description 	
	looseandsketchyoffline.xml
	)

include( add_project )
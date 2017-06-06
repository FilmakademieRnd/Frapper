project(looseandsketchy)

# looseandsketchy
set( res_header 		
	LooseAndSketchyNode.h
	LooseAndSketchyNodePlugin.h
	Vector2d.h
	)

set( res_moc 	
	LooseAndSketchyNode.h
	LooseAndSketchyNodePlugin.h
	)

set( res_source 	
	LooseAndSketchyNode.cpp
	LooseAndSketchyNodePlugin.cpp
	)

set( res_description 	
	looseandsketchy.xml
	)

include( add_project )
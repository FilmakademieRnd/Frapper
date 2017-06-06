project(looseandsketchy2)

# looseandsketchy2
set( res_header 		
	LooseAndSketchy2Node.h
	LooseAndSketchy2NodePlugin.h
	Vector2d.h
	)

set( res_moc 	
	LooseAndSketchy2Node.h
	LooseAndSketchy2NodePlugin.h
	)

set( res_source 	
	LooseAndSketchy2Node.cpp
	LooseAndSketchy2NodePlugin.cpp
	)

set( res_description 	
	looseandsketchy2.xml
	)

include( add_project )
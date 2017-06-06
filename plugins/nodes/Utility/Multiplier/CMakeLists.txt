project(multiplier)

# multiplier
set( res_header
			MultiplierNode.h
			MultiplierNodePlugin.h
			)

set( res_moc
			MultiplierNode.h
			MultiplierNodePlugin.h
			)

set( res_source
			MultiplierNode.cpp
			MultiplierNodePlugin.cpp
			)

set( res_description
			multiplier.xml
			)

include( add_project )
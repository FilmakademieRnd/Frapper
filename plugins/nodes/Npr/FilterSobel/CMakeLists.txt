project(filtersobel)

# filtersobel
set( res_header
			FilterSobelNode.h
			FilterSobelNodePlugin.h
			)

set( res_moc
			FilterSobelNode.h
			FilterSobelNodePlugin.h
			)

set( res_source
			FilterSobelNode.cpp
			FilterSobelNodePlugin.cpp
			)

set( res_description
			filtersobel.xml
			)

include( add_project )
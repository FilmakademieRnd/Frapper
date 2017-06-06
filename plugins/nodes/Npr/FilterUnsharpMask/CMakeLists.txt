project(filterunsharpmask)

# filterunsharpmask
set( res_header
			FilterUnsharpMaskNode.h
			FilterUnsharpMaskNodePlugin.h
			)

set( res_moc
			FilterUnsharpMaskNode.h
			FilterUnsharpMaskNodePlugin.h
			)

set( res_source
			FilterUnsharpMaskNode.cpp
			FilterUnsharpMaskNodePlugin.cpp
			)

set( res_description
			filterunsharpmask.xml
			)

include( add_project )
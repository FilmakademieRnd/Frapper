project(colorcorrection)

# blend
set( res_header
			ColorCorrectionNode.h
			ColorCorrectionNodePlugin.h
			)

set( res_moc
			ColorCorrectionNode.h
			ColorCorrectionNodePlugin.h
			)

set( res_source
			ColorCorrectionNode.cpp
			ColorCorrectionNodePlugin.cpp
			)

set( res_description
			colorcorrection.xml
			)

include( add_project )
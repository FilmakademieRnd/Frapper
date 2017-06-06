project(filtermorphologicalsmooth)

# filtermorphologicalsmooth
set( res_header
			FilterMorphologicalSmoothNode.h
			FilterMorphologicalSmoothNodePlugin.h
			)

set( res_moc
			FilterMorphologicalSmoothNode.h
			FilterMorphologicalSmoothNodePlugin.h
			)

set( res_source
			FilterMorphologicalSmoothNode.cpp
			FilterMorphologicalSmoothNodePlugin.cpp
			)

set( res_description
			filtermorphologicalsmooth.xml
			)

include( add_project )
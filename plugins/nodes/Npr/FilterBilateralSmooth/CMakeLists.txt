project(filterbilateralsmooth)

# filterbilateralsmooth
set( res_header
			FilterBilateralSmoothNode.h
			FilterBilateralSmoothNodePlugin.h
			)

set( res_moc
			FilterBilateralSmoothNode.h
			FilterBilateralSmoothNodePlugin.h
			)

set( res_source
			FilterBilateralSmoothNode.cpp
			FilterBilateralSmoothNodePlugin.cpp
			)

set( res_description
			filterbilateralsmooth.xml
			)

include( add_project )
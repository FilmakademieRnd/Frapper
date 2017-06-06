project(OpenMvgFeatureFinder)

set( res_header
	OpenMvgFeatureFinder.h
	OpenMvgFeatureFinderPlugin.h
	)

set( res_moc
	OpenMvgFeatureFinder.h
	OpenMvgFeatureFinderPlugin.h
	)

set( res_source
	OpenMvgFeatureFinder.cpp
	OpenMvgFeatureFinderPlugin.cpp
	)

set( res_description
	OpenMvgFeatureFinder.xml
	)
	
include( add_project )
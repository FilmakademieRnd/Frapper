project(OpenMvgGlobalSfM)

set( res_header
	OpenMvgGlobalSfM.h
	OpenMvgGlobalSfMPlugin.h
	)

set( res_moc
	OpenMvgGlobalSfM.h
	OpenMvgGlobalSfMPlugin.h
	)

set( res_source
	OpenMvgGlobalSfM.cpp
	OpenMvgGlobalSfMPlugin.cpp
	)

set( res_description
	OpenMvgGlobalSfM.xml
	)
	
include( add_project )
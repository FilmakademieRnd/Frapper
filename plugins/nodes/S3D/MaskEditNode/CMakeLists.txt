project(MaskEdit)

set( res_header 		
	MaskEditNode.h
	MaskEditNodePlugin.h
	../CImgListTools/CImgListTools.h
	../CharonInterface/CharonInterface.h
	)

set( res_moc 	
	MaskEditNode.h
	MaskEditNodePlugin.h
	../CharonInterface/CharonInterface.h
	)

set( res_source 	
	MaskEditNode.cpp
	MaskEditNodePlugin.cpp
	../CImgListTools/CImgListTools.hpp
	../CharonInterface/CharonInterface.hpp
	)

set( res_description 	
	MaskEditNode.xml
	)

FIND_PACKAGE(charon-core REQUIRED)
FIND_PACKAGE(charon-utils REQUIRED)

# get include directores of painter panel
get_property( painterpanel_INCLUDE_DIRS GLOBAL PROPERTY painterpanel_INCLUDE_DIRS )
get_property( painterpanel_LIBRARIES GLOBAL PROPERTY painterpanel_LIBRARIES )

if( charon-core_FOUND AND charon-utils_FOUND )

	set ( add_include_dir
		${charon-core_INCLUDE_DIRS}
		${charon-utils_INCLUDE_DIRS}
		${painterpanel_INCLUDE_DIRS}
		${CIMG_INCLUDE_DIRS}
	)

	# Add library dependencies
	set( add_link_lib
		charon-core
		charon-plugins
		${painterpanel_LIBRARIES}
		${CIMG_LINK_LIBRARIES}
	)
	
	include( add_project )

else( charon-core_FOUND AND charon-utils_FOUND )
    message( "charon-core and/or charon-utils not found! Skipping project ${CMAKE_PROJECT_NAME}!" )
endif( charon-core_FOUND AND charon-utils_FOUND )

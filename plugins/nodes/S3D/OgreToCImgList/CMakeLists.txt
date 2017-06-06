project(ogreToCImgList)

set( res_header 		
	OgreToCImgListNode.h
	OgreToCImgListNodePlugin.h
	)

set( res_moc 	
	OgreToCImgListNode.h
	OgreToCImgListNodePlugin.h
	)

set( res_source 	
	OgreToCImgListNode.cpp
	OgreToCImgListNodePlugin.cpp
	)

set( res_description 	
	ogreToCImgList.xml
	)

FIND_PACKAGE(charon-core REQUIRED)
FIND_PACKAGE(charon-utils REQUIRED)

if( charon-core_FOUND AND charon-utils_FOUND )

	set ( add_include_dir
		${charon-core_INCLUDE_DIRS}
		${charon-utils_INCLUDE_DIRS}
		${CIMG_INCLUDE_DIRS}

	)

	# Add library dependencies
	set( add_link_lib
		charon-core
		charon-plugins
		${CIMG_LINK_LIBRARIES}
	)

	include( add_project )

else( charon-core_FOUND AND charon-utils_FOUND )
    message( "charon-core and/or charon-utils not found! Skipping project ${CMAKE_PROJECT_NAME}!" )
endif( charon-core_FOUND AND charon-utils_FOUND )
 # If Qt4 was used, add the 'found' Qt-libraries to the Install-target.
 
message( STATUS "QT_FOUND = ${QT_FOUND}")
message( STATUS "FRAPPER_INSTALL_QT = ${FRAPPER_INSTALL_QT}")

if ( QT_FOUND AND FRAPPER_INSTALL_QT )
    foreach( Qt_library ${QT_LIBRARIES} )

		message( STATUS "Qt_library = ${Qt_library}")
		# With QT_USE_IMPORTED_TARGETS, we should extract the dll info 
        # from the target properties
        get_target_property( Qt_lib_name ${Qt_library} IMPORTED_LOCATION )
		message( STATUS "Qt_lib_name = ${Qt_lib_name}")
        get_target_property( Qt_lib_name_debug ${Qt_library} IMPORTED_LOCATION_DEBUG )
		message( STATUS "Qt_lib_name_debug = ${Qt_lib_name_debug}")
        get_target_property( Qt_lib_name_release ${Qt_library} IMPORTED_LOCATION_RELEASE )
		message( STATUS "Qt_lib_name_release = ${Qt_lib_name_release}")

        # Initially assume the release dlls should be installed, but 
        # fall back to debug if necessary
        if ( Qt_lib_name_release AND EXISTS ${Qt_lib_name_release} )
            set( Qt_library_location ${Qt_lib_name_release} )
        elseif ( Qt_lib_name_debug AND EXISTS ${Qt_lib_name_debug} AND ENVIRONMENT_DEBUG )
            set( Qt_library_location ${Qt_lib_name_debug} )
        elseif ( Qt_lib_name AND EXISTS ${Qt_lib_name} )
            set( Qt_library_location ${Qt_lib_name} )
        endif ( Qt_lib_name_release AND EXISTS ${Qt_lib_name_release} )

        # Extract the filename part, without the lib-prefix or the .a or ..lib suffix

		message( STATUS "Qt_library_location = ${Qt_library_location}")
        get_filename_component( Qt_library_name ${Qt_library_location} NAME_WE )
        string( REGEX REPLACE "^lib(.*)" "\\1" Qt_library_name ${Qt_library_name} )

        set( Qt_shared_library ${QT_BINARY_DIR}/${Qt_library_name}.dll )
        if ( EXISTS ${Qt_shared_library} )
            # Add it to the list of 'desired' qt-libraries for later installation
            list( APPEND Qt_Install_Libraries ${Qt_shared_library} )
        else ( EXISTS ${Qt_shared_library} )
            message( WARNING "    could not find ${Qt_shared_library}" )
        endif ( EXISTS ${Qt_shared_library} )
    endforeach( Qt_library ${QT_LIBRARIES} )
    # When building against a static Qt, the list of Qt_Install_Libraries can be empty
	message( STATUS "Qt_Install_Libraries = ${Qt_Install_Libraries}")
    if ( Qt_Install_Libraries )
        list( REMOVE_DUPLICATES Qt_Install_Libraries )
        install( FILES ${Qt_Install_Libraries} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ThirdParty )
    endif ( Qt_Install_Libraries )
endif ( QT_FOUND AND FRAPPER_INSTALL_QT )

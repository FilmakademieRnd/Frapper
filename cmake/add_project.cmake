## Include this script in a CMakeLists.txt script to add 
## a project to the solution using pre-defined file sets.
##
##   Define the file-sets using the following template:
##   
##   project( ProjectName )
##   
##   # header files
##   set( res_header
##    ...
##   )
##   
##   # source files
##   set( res_source
##	  ...
##   )
##   
##   # files to compile with qt meta object compiler
##   set( res_moc 
##    ...
##   )
##   
##   # files to compile with user interface compiler
##   set( res_ui
##    ...
##   )
##   
##   # files to compile with resource compiler
##   set( res_qrc
##    ...
##   )
##   
##   # shader files to install to frapper resources folder
##   set( res_shader
##    ...
##   )
##   
##   # node description file
##   set( res_description 
##    ...
##   )
##
##   # additional resource files to install in folder of binary
##   set( res_additional
##    ...
##   )
##
##   # additional resource files to install in frapper binary folder, e.g. /bin/win32 or /bin/x64
##   set( res_bin
##    ...
##   )
##
##   # additional include directories
##   set( add_include_dir
##    ...
##   )
##
##   # additional link directories
##   set( add_link_dir
##    ...
##   )
##
##   # additional libraries to link against, e.g.
##   set( add_link_lib
##		  optimized frappergui debug frappergui_d
##        ...
##   )
##	 # The following libraries are always linked:
##     ${QT_LIBRARIES}
##     ${OGRE_LIBRARIES}
##     optimized frappercore debug frappercore_d
##
##
##	 # add project as executable instead of library (default)
##	 set( create_executable TRUE)
##
##   include( add_project )
##
## For shader nodes you can use the following template:
##   
##   project( ExampleShading)
##   
##   # template description file
##   set( res_description 
##   	exampleShading.xml
##   )
##   
##   # Shader files and resources used by shaders
##   set( res_shader
##       shader/Example.cg
##       shader/Example.compositor
##       shader/Example.material
##       shader/Texture.png
##   )
##
##   include( add_project )
## 
## 

# Create moc files
if (FRAPPER_USE_QT5)
  if( res_moc )
      qt5_wrap_cpp( res_cxx ${res_moc})
  endif()

  if( res_ui )
    qt5_wrap_ui( res_uih ${res_ui})
  endif()

  if( res_qrc )
    qt5_add_resources( res_rcc ${res_qrc})
  endif()
else()
  if( res_moc )
      qt4_wrap_cpp( res_cxx ${res_moc})
  endif()

  if( res_ui )
    qt4_wrap_ui( res_uih ${res_ui})
  endif()

  if( res_qrc )
    qt4_add_resources( res_rcc ${res_qrc})
  endif()
endif()

# Create (optional) source groups
if( res_cxx OR res_uih OR res_rcc )
    source_group( "Generated Files" FILES ${res_cxx} ${res_uih} ${res_rcc})
endif()

if( res_header )
    source_group( "Header Files" FILES ${res_header})
endif()

if( res_description )
    source_group( "Description" FILES ${res_description})
endif()

if( res_shader)
    source_group( "Shader" FILES ${res_shader})
endif()

if( res_additional )
    source_group( "Resources" FILES ${res_additional})
endif()

if( res_ui )
	source_group( "Ui Files" FILES ${res_ui} )
endif()

if( res_qrc )
	source_group( "Resource Files" FILES ${res_ui} )
endif()

# set install directory of plugin   
if( ispanel )
	set( INSTALL_DIR ${PANEL_PLUGIN_INSTALL} )
elseif( iswidget )
	set( INSTALL_DIR ${WIDGET_PLUGIN_INSTALL} )
else()
	set( INSTALL_DIR ${NODE_PLUGIN_INSTALL} )
endif()

if( res_header )
	# store project include directories to include in other projects
	set_property( GLOBAL PROPERTY ${PROJECT_NAME}_INCLUDE_DIRS 
		${CMAKE_CURRENT_SOURCE_DIR} 
        ${add_include_dir}
	)
endif()
   
# Check if source files are available -> Create static library
if( res_source )

    set(CMAKE_DEBUG_POSTFIX _d)

	# include current source directory
    include_directories( 
		${CMAKE_CURRENT_SOURCE_DIR} 
		${CMAKE_CURRENT_BINARY_DIR}
        ${add_include_dir} )

	link_directories(
        ${add_link_dir}
    )
	
	# store project libraries to link in other projects
	set_property( GLOBAL PROPERTY ${PROJECT_NAME}_LIBRARIES
		${PROJECT_NAME}
        ${add_link_lib}
	)

    # Add all files to sources to make headers visible in Visual Studio
    if( WIN32 )
        set( res_source ${res_source} ${res_header} ${res_description} 
		                ${res_shader} ${res_additional} ${res_ui} ${res_qrc})
    endif()

	# check if target was already defined, e.g. as executable
	if( create_executable )
		add_executable( ${PROJECT_NAME} ${res_source} ${res_cxx} ${res_uih} ${res_rcc})
		# workaround because set(CMAKE_DEBUG_POSTFIX _d) 
		# does not affect executables
		set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_OUTPUT_NAME "${PROJECT_NAME}_d" )
		install( TARGETS ${PROJECT_NAME} 
				 RUNTIME 
				 DESTINATION ${CMAKE_INSTALL_PREFIX} 
				 COMPONENT Executables )
	else()
		add_library( ${PROJECT_NAME} SHARED ${res_source} ${res_cxx} ${res_uih} ${res_rcc})
		
	install( TARGETS ${PROJECT_NAME} 
			 RUNTIME 
			 DESTINATION ${INSTALL_DIR} 
			 COMPONENT Plugins )
	endif()

    # Add library dependencies
	if( NOT iscore)
		set( add_link_lib 
			 ${add_link_lib} 
			 optimized frappercore debug frappercore_d 
		)
	endif()

	target_link_libraries( ${PROJECT_NAME}
						   ${QT_LIBRARIES}
						   ${OGRE_LIBRARIES}
						   ${add_link_lib}
	)
else()

    # No sources given ->
    # Create custom target for template nodes defined by xml-description file
    add_custom_target( ${PROJECT_NAME} ALL SOURCES ${res_description} ${res_shader} ) 
endif()

# Install additional files & shaders
install( FILES ${res_description} DESTINATION ${INSTALL_DIR} )
install( FILES ${res_additional}  DESTINATION ${RESOURCES_INSTALL} )
install( FILES ${res_bin}         DESTINATION ${CMAKE_INSTALL_PREFIX} )
install( FILES ${res_shader}      DESTINATION ${RESOURCES_INSTALL}/${PROJECT_NAME}_Shader )
   
# Deprecated MinGW stuff
# if (MINGW)
#     if (CMAKE_BUILD_TYPE MATCHES Debug)
#         install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgeometryblur_d.dll DESTINATION ${NODE_PLUGIN_INSTALL} )
#     else (CMAKE_BUILD_TYPE MATCHES Debug)
#         install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgeometryblur.dll DESTINATION ${NODE_PLUGIN_INSTALL} )
#     endif (CMAKE_BUILD_TYPE MATCHES Debug)
# else (MINGW)
#     install( TARGETS geometryblur RUNTIME DESTINATION ${NODE_PLUGIN_INSTALL} )
# endif (MINGW)

# Set Solution folder in Visual Studio
if( FRAPPER_SOLUTION_USE_FOLDERS AND VS_PROJECT_FOLDER )
	set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER ${VS_PROJECT_FOLDER})
	message( STATUS " * Adding project ${PROJECT_NAME} to folder ${VS_PROJECT_FOLDER}")
else()
	message( STATUS " * Adding project ${PROJECT_NAME}")
endif()

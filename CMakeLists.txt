
cmake_minimum_required(VERSION 2.8.3)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

#suppress CMP0020 warnings
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 OLD)
endif()


# build debug
IF( FRAPPER_BUILD_DEBUG )
SET ( CMAKE_BUILD_TYPE "Debug" )
SET ( CMAKE_INSTALL_CONFIG_NAME "Debug" )
ADD_DEFINITIONS(-D_DEBUG)
ELSE( FRAPPER_BUILD_DEBUG )
SET ( CMAKE_BUILD_TYPE "Release" )
SET ( CMAKE_INSTALL_CONFIG_NAME "Release" )
ENDIF( FRAPPER_BUILD_DEBUG )

# add our cmake folder to cmake module path
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

# Detect 64 bit 
IF( CMAKE_CL_64 OR CMAKE_SYSTEM_PROCESSOR MATCHES "[xX]64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "[xX]86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "[aA][mM][dD]64" OR CMAKE_GENERATOR MATCHES Win64*)
	SET( IS_X64 TRUE )
ENDIF()

# set install dirs
IF( IS_X64)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin/x64 CACHE PATH "Default Frapper install prefix" FORCE )
ELSE( IS_X64)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/bin/win32 CACHE PATH "Default Frapper install prefix" FORCE )
ENDIF( IS_X64)

# backup current path environment variable
SET(PATH_BACKUP "$ENV{PATH}")

# Check if precompiled third-party libraries like collada are available..
IF( IS_X64)
	SET( THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty_libraries_x64" )
ELSE( IS_X64)
	SET( THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty_libraries")
ENDIF( IS_X64)

# find collada 1.5 lib
IF( IS_DIRECTORY "${THIRDPARTY_DIR}" )
	FILE(GLOB COLLADA_DOM_LIB_DEBUG "${THIRDPARTY_DIR}/collada/lib/*collada15dom*d.lib" )
	FILE(GLOB COLLADA_DOM_LIB "${THIRDPARTY_DIR}/collada/lib/*collada15dom*[^d].lib" )
ELSE()
	MESSAGE( FATAL_ERROR "Third-party libraries missing! Please download them from http://sourceforge.net/projects/frapper/files/frapper-thirdparty-libraries and extract them into your Frapper root directory!" )
ENDIF()

# Find OGRE; we are using OGRE > 1.7
# Try to read OGRE_DIR from OGRE_HOME environment variable (only in the first run!)
IF (NOT OGRE_DIR OR OGRE_DIR STREQUAL "")
  IF(IS_X64)
    SET(OGRE_DIR $ENV{OGRE_HOME_64} CACHE PATH "Path to the OGRE SDK (64 bit).")
  ELSE()
    SET(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Path to the OGRE SDK.")
  ENDIF()
ENDIF()

# Append Ogre directory name to project name for easier distinction
option( FRAPPER_SOLUTION_ADD_OGRENAME "Append name of Ogre directory to project for easier distinction" FALSE ) 

# Try estimating ogre version from directory name:
IF( FRAPPER_SOLUTION_ADD_OGRENAME )
  GET_FILENAME_COMPONENT( OGRE_DIRNAME ${OGRE_DIR} NAME )
  MESSAGE( STATUS "Setting project name to frapper_${OGRE_DIRNAME}")
  PROJECT(frapper_${OGRE_DIRNAME})
ELSE()
  PROJECT(frapper)
ENDIF()

# Now try to find OGRE within the given directory
IF( IS_DIRECTORY ${OGRE_DIR} )
	# Set module-path to locate findOGRE.cmake
	SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OGRE_DIR}/CMake )

	# Set OGRE_HOME if it wasn't already
	IF (NOT ENV{OGRE_HOME} OR ENV{OGRE_HOME} STREQUAL "")
		SET (ENV{OGRE_HOME} ${OGRE_DIR} )
	ENDIF()

	# find OGRE
	FIND_PACKAGE( OGRE REQUIRED )
	IF( OGRE_FOUND )
        INCLUDE_DIRECTORIES( 
			${OGRE_INCLUDE_DIRS} 
			${OGRE_INCLUDE_DIRS}/OIS
			${OGRE_Overlay_INCLUDE_DIRS}
		)
	ENDIF()
ELSE()
	MESSAGE( FATAL_ERROR "OGRE not found! OGRE is required to build frapper. Please specify the path where OGRE is located or set the OGRE_HOME variable." )
ENDIF()

IF (${OGRE_VERSION} LESS "2.10.0")
	# Find Boost, required by OGRE

	message(STATUS "OGRE requires Boost. Trying to find Boost..")

	# Try to find Boost using the Boost_DIR evironment variable (only in the first run!)
	IF (NOT BOOST_PATH OR BOOST_PATH STREQUAL "")
		SET(BOOST_PATH $ENV{Boost_DIR} CACHE PATH "Path to the Boost SDK.")
	ENDIF()

	# Try to use Boost from the OGRE SDK in case BOOST_PATH isn't set
	IF ( BOOST_PATH STREQUAL "")
		IF (IS_DIRECTORY ${OGRE_DIR}/boost)
			SET (BOOST_PATH ${OGRE_DIR}/boost)
		ELSEIF (IS_DIRECTORY ${OGRE_DIR}/boost_1_42)
			SET (BOOST_PATH ${OGRE_DIR}/boost_1_42)
		ELSEIF (IS_DIRECTORY ${OGRE_DIR}/boost_1_44)
			SET (BOOST_PATH ${OGRE_DIR}/boost_1_44)
		ELSEIF (IS_DIRECTORY ${OGRE_DIR}/boost_1_47)
			SET (BOOST_PATH ${OGRE_DIR}/boost_1_47)
		ELSEIF (IS_DIRECTORY ${OGRE_DIR}/boost_1_48)
			SET (BOOST_PATH ${OGRE_DIR}/boost_1_48)
		ENDIF()
	ENDIF ()

	# check if a valid directory was found
	IF( IS_DIRECTORY ${BOOST_PATH} )

	# Communicate Boost's location to the find script
	SET (ENV{Boost_DIR} ${BOOST_PATH} )
	SET (ENV{BOOST_ROOT} ${BOOST_PATH} )

	# locate Boost using FIND_PACKAGE
		SET (Boost_NO_SYSTEM_PATHS	  ON)
		SET (Boost_USE_STATIC_LIBS        ON)
		SET (Boost_USE_MULTITHREADED      ON)
		SET (Boost_USE_STATIC_RUNTIME    OFF)
		FIND_PACKAGE( Boost COMPONENTS system )
		FIND_PACKAGE( Boost COMPONENTS chrono )
		FIND_PACKAGE( Boost REQUIRED COMPONENTS thread date_time system chrono )
		IF(Boost_FOUND)
			INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
			MESSAGE(STATUS "Boost found at $ENV{Boost_DIR}")
			# Add boost to OGRE libraries
			SET(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
		ENDIF()
	#ELSE()
		#MESSAGE( FATAL_ERROR "Boost not found! Please specify the path where Boost is located or set the Boost_DIR variable." )
	ENDIF()
		#IF(Boost_FOUND AND OGRE_FOUND)
		IF(OGRE_FOUND)
		# Add boost to OGRE libraries
		SET(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
	ENDIF()
ENDIF()

# Find Qt
IF (NOT QT_DIR OR QT_DIR STREQUAL "")
  IF(IS_X64)
    SET(QT_DIR $ENV{QTDIR_64} CACHE PATH "Path to the Qt SDK (64 bit).")
  ELSE()
    SET(QT_DIR $ENV{QTDIR} CACHE PATH "Path to the Qt SDK.")
  ENDIF()
ENDIF()
SET(ENV{PATH} ${QT_DIR}/bin)

option( FRAPPER_USE_QT5 "Build with Qt5." FALSE )
IF(FRAPPER_USE_QT5)
  # FindQt5
  FIND_PACKAGE(Qt5Core)
  FIND_PACKAGE(Qt5Gui)
  FIND_PACKAGE(Qt5Multimedia)
  FIND_PACKAGE(Qt5Network)
  FIND_PACKAGE(Qt5Widgets)
  FIND_PACKAGE(Qt5Xml)
  FIND_PACKAGE(Qt5Concurrent)
  if (Qt5UiPlugin_FOUND)
        # for some reason, Qt5UiPlugin does not set its _INCLUDE_DIRS variable. Fill it manually
        get_target_property(Qt5UiPlugin_INCLUDE_DIRS Qt5::UiPlugin INTERFACE_INCLUDE_DIRECTORIES)
  endif()
  SET(QT_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES})
  SET(QT_MULTIMEDIA ${Qt5Network_LIBRARIES})
  SET(QT_NETWORK ${Qt5Network_LIBRARIES})
  SET(QT_CONCURRENT ${Qt5Concurrent_LIBRARIES})
  SET(QT_INCLUDES ${Qt5Widgets_INCLUDES} ${Qt5Xml_INCLUDES} ${Qt5Concurrent_INCLUDES})
  INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
  add_definitions(${Qt5Widgets_DEFINITIONS})
  set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
ELSE() 
  # FindQt4 needs qmake.exe in the path
  FIND_PACKAGE(Qt4 REQUIRED COMPONENTS QtCore QtGui QtXml)
  INCLUDE_DIRECTORIES(${QT_QTCORE_INCLUDE_DIR})
  INCLUDE(${QT_USE_FILE})
  SET(ENV{PATH} "${PATH_BACKUP}")
ENDIF()




# Set Visual Studio specific build flags
IF (MSVC)
	IF (MSVC_VERSION LESS 1800)
		SET ( CMAKE_CXX_FLAGS_RELEASE  "/Ox /Ob2 /Oi /Ot /GT /arch:SSE2" )
	ELSE ()
		SET ( CMAKE_CXX_FLAGS_RELEASE  "/Ox /Ob2 /Oi /Ot /GT /arch:AVX" )
	ENDIF ()
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc") #/EHsc to avoid boost throw::exeption linker errors 
ENDIF ()

# global include directories
INCLUDE_DIRECTORIES(
	#${frappercore_SOURCE_DIR} 
	#${frappergui_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/core
	${CMAKE_SOURCE_DIR}/gui
	${CMAKE_BINARY_DIR}/core
	${CMAKE_BINARY_DIR}/gui
	${THIRDPARTY_DIR}/collada/include
	${THIRDPARTY_DIR}/collada/include/1.5
	)
		
# global link directories
LINK_DIRECTORIES(
	${CMAKE_BINARY_DIR}/core
	${CMAKE_BINARY_DIR}/gui
	${QT_LIBRARY_DIR}
)

# build options
option( FRAPPER_BUILD_DEMO "Build Demo for Sourceforge release. Overrides any option." FALSE ) 
if(UNIX)
option( FRAPPER_BUILD_DEBUG "Build Debug Configuration (for MingGW only)." FALSE )
endif(UNIX)
option( FRAPPER_BUILD_NODES_ANIMATION "Build Animation nodes." TRUE )
option( FRAPPER_BUILD_NODES_CAMERAS "Build Camera nodes." TRUE )
option( FRAPPER_BUILD_NODES_COMPOSITING "Build Compositing nodes." TRUE )
option( FRAPPER_BUILD_NODES_DATAEXCHANGE "Build Export nodes." FALSE )
option( FRAPPER_BUILD_NODES_GEOMETRY "Build Geometry nodes." TRUE )
option( FRAPPER_BUILD_NODES_IMAGES "Build Image nodes." TRUE )
option( FRAPPER_BUILD_NODES_INPUT "Build Input nodes." TRUE )
option( FRAPPER_BUILD_NODES_LIGHTS "Build Light nodes." TRUE )
option( FRAPPER_BUILD_NODES_NETWORK "Build Network nodes." TRUE )
option( FRAPPER_BUILD_NODES_NPR "Build NPR nodes." TRUE )
option( FRAPPER_BUILD_NODES_RENDERING "Build Rendering nodes." TRUE )
option( FRAPPER_BUILD_NODES_SHADING "Build Shading nodes." TRUE )
option( FRAPPER_BUILD_NODES_S3D "Build S3D nodes." FALSE )
option( FRAPPER_BUILD_NODES_TIMER "Build Timer nodes." TRUE )
option( FRAPPER_BUILD_NODES_TRACKING "Build Tracking nodes." FALSE )
option( FRAPPER_BUILD_NODES_UTILITY "Build Utility nodes." TRUE )
option( FRAPPER_BUILD_NODES_PYTHON "Build Python node." FALSE )

# only display this when additional_nodes directory present
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/nodes_additional)
option( FRAPPER_BUILD_NODES_ADDITIONAL "Build additional nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_AF "Build additional Agent Framework nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_AF_ALL "Build all additional Agent Framework nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_AF_DIALOGSYSTEM "Build only Dialog System nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_AF_EMOTE "Build only EMOTE Messaging Server nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_CUDACLIPBAKER "Build cuda clip baker." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_DROPLET "Build all additional droplet nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_S3D "Build additional S3D Charon nodes." FALSE )
option( FRAPPER_BUILD_NODES_S3DGAME "Build S3D Game nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_SOLVER "Build additional Stereo 3D nodes." FALSE )
option( FRAPPER_BUILD_NODES_ADDITIONAL_TRACKING "Build additional TRACKING nodes." FALSE )
endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/nodes_additional)

option( FRAPPER_BUILD_PANELS "Build Panel plugins." TRUE )
option( FRAPPER_BUILD_WIDGETS "Build Widget plugins." FALSE )
option( FRAPPER_BUILD_TRISTEREO "Build Tridelity Stereo support" FALSE )
option( FRAPPER_BUILD_APPLICATIONS_STEREOBOTTIC "Build Stereobottic Application." FALSE )

option( FRAPPER_SOLUTION_USE_FOLDERS "Arrange node projects in solution folders. Not available in VS Express!" TRUE)
SET_PROPERTY ( GLOBAL PROPERTY USE_FOLDERS ${FRAPPER_SOLUTION_USE_FOLDERS} ) 

option( FRAPPER_LOG_EVALUATION "Enable logging of parameter evaluation." FALSE)
option( FRAPPER_HIDE_CONSOLE_IN_RELEASE "If enabled, executing will not show up console when building Release or RelWithDebInfo configurations." FALSE)

#install qt & ogre runtime libraries (dlls)
option( FRAPPER_INSTALL_QT "Install Qt runtime libraries to bin folder" TRUE)
option( FRAPPER_INSTALL_OGRE "Install Ogre runtime libraries to bin folder" TRUE)

set ( NODE_PLUGIN_INSTALL plugins/nodes )
set ( PANEL_PLUGIN_INSTALL plugins/panels )
set ( WIDGET_PLUGIN_INSTALL plugins/widgets )
set ( RESOURCES_INSTALL resources )

# add sub-folders
add_subdirectory(applications)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(plugins)

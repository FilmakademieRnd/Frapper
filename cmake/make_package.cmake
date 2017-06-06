
set(CPACK_PACKAGE_NAME "Frapper")
set(CPACK_PACKAGE_VENDOR "www.Filmakademie.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Frapper - The Filmakademie Application Framework")  
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "FrapperDist")

#include( install_qt_binaries ) 
#include( install_ogre_binaries ) 

set( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include( InstallRequiredSystemLibraries)
install( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}
         COMPONENT ThirdParty)
 
set(CPACK_COMPONENTS_ALL Executables Plugins ThirdParty)
set(CPACK_COMPONENT_PLUGINS_DEPENDS Executables)
set(CPACK_COMPONENT_EXECUTABLES_DEPENDS ThirdParty)

# This must always be last!
#include(CPack)

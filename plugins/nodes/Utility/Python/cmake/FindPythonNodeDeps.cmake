# - Find Python Node Dependencies
#
MESSAGE(STATUS "Checking Python Node Dependencies")

INCLUDE(FrapperFindPackage)
FIND_PACKAGE(PythonInterp)

IF(PYTHONINTERP_FOUND)
    # check that Python version 2.x is used
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                         "import sys; print sys.version[0]"
                          OUTPUT_VARIABLE PYTHON_MAJOR_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    IF(${PYTHON_MAJOR_VERSION} EQUAL "2")
        SET(PYTHONINTERP_V2_FOUND 1)
    ELSE()
        MESSAGE(STATUS "python node currently requires Python 2.x.")
        MESSAGE(STATUS "Make sure that Python 2 is in your PATH or use 'cmake_gui' to set the PYTHON_EXECUTABLE variable manually.")
        SET(PYTHONINTERP_V2_FOUND 0)
    ENDIF()
ELSE()
    SET(PYTHONINTERP_V2_FOUND 0)
ENDIF()

IF(PYTHONINTERP_V2_FOUND)

#    this command cannot be used because its results are often inconsistent
#    with the Python interpreter found previously (e.g. libraries or includes
#    from incompatible installations)
#    FIND_PACKAGE(PythonLibs)

    # find Python library
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                     "import sys; print sys.exec_prefix"
                      OUTPUT_VARIABLE PYTHON_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)

    IF(APPLE AND ${PYTHON_PREFIX} MATCHES ".*framework.*")
        SET(PYTHON_LIBRARIES "${PYTHON_PREFIX}/Python"
            CACHE FILEPATH "Python libraries"
            FORCE)
    ELSE()
        execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                         "import sys; skip = 2 if sys.platform.startswith('win') else 1; print 'python' + sys.version[0:3:skip]"
                          OUTPUT_VARIABLE PYTHON_LIBRARY_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)
        FIND_LIBRARY(PYTHON_LIBRARIES ${PYTHON_LIBRARY_NAME} PATHS "${PYTHON_PREFIX}" 
                     PATH_SUFFIXES lib lib64 libs DOC "Python libraries" NO_DEFAULT_PATH)
    ENDIF()

    # find Python includes
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                    "from distutils.sysconfig import *; print get_python_inc()"
                    OUTPUT_VARIABLE PYTHON_INCLUDE OUTPUT_STRIP_TRAILING_WHITESPACE)
    SET(PYTHON_INCLUDE_PATH ${PYTHON_INCLUDE}
        CACHE PATH "Path to Python include files"
        FORCE)

    IF(PYTHON_LIBRARIES AND PYTHON_INCLUDE_PATH)
        MESSAGE(STATUS "Found Python libraries: ${PYTHON_LIBRARIES}")
        MESSAGE(STATUS "Found Python includes:  ${PYTHON_INCLUDE_PATH}")
        SET(PYTHONLIBS_FOUND TRUE)
    ELSE()
        MESSAGE(STATUS "Could NOT find Python libraries and/or includes")
    ENDIF()

	
	# locate Boost using FIND_PACKAGE
	FIND_PACKAGE( Boost REQUIRED COMPONENTS python )

    ######################################################################
    #
    #      find default install directory for Python modules
    #      (usually PYTHONDIR/Lib/site-packages)
    #
    ######################################################################
    IF(NOT DEFINED PYTHONNODE_INSTALL_DIR OR PYTHONNODE_INSTALL_DIR STREQUAL "")
        execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                         "from distutils.sysconfig import *; print get_python_lib(1)"
                          OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
        FILE(TO_CMAKE_PATH ${PYTHON_SITE_PACKAGES} PYTHONNODE_INSTALL_DIR)
    ENDIF()
    SET(PYTHONNODE_INSTALL_DIR ${PYTHONNODE_INSTALL_DIR}
        CACHE PATH "where to install the FRAPPER Python package" FORCE)
    # this is the install path relative to CMAKE_INSTALL_PREFIX,
    # use this in INSTALL() commands to get packaging right
    FILE(RELATIVE_PATH PYTHONNODE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} ${PYTHONNODE_INSTALL_DIR})

    ######################################################################
    #
    #      find numpy include directory
    #      (usually below PYTHONDIR/Lib/site-packages/numpy)
    #
    ######################################################################
    # IF(NOT PYTHON_NUMPY_INCLUDE_DIR)
        # # Note: we must suppress possible output of the 'from numpy... import *' command,
        # #       because the output cannot be interpreted correctly otherwise
        # execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                         # "import sys, os; sys.stdout = open(os.devnull, 'w'); from numpy.distutils.misc_util import *; sys.__stdout__.write(' '.join(get_numpy_include_dirs()))"
                          # RESULT_VARIABLE PYTHON_NUMPY_NOT_FOUND
                          # OUTPUT_VARIABLE PYTHON_NUMPY_INCLUDE_DIR
                          # OUTPUT_STRIP_TRAILING_WHITESPACE)
        # IF(NOT PYTHON_NUMPY_NOT_FOUND)
            # FILE(TO_CMAKE_PATH ${PYTHON_NUMPY_INCLUDE_DIR} PYTHON_NUMPY_INCLUDE_DIR)
        # ELSE()
            # SET(PYTHON_NUMPY_INCLUDE_DIR "PYTHON_NUMPY_INCLUDE_DIR-NOTFOUND")
        # ENDIF()
    # ENDIF()

    # SET(PYTHON_NUMPY_INCLUDE_DIR ${PYTHON_NUMPY_INCLUDE_DIR}
        # CACHE PATH "Path to numpy include files" FORCE)
    # IF(PYTHON_NUMPY_INCLUDE_DIR)
        # MESSAGE(STATUS "Searching for Python numpy: ok")
    # ELSE()
        # MESSAGE(STATUS "Could NOT find Python numpy ('import numpy.distutils.misc_util' failed)")
    # ENDIF()

    ######################################################################
    #
    #      check if nosetests are installed
    #
    ######################################################################
    # execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                     # "import nose"
                      # RESULT_VARIABLE PYTHON_NOSETESTS_NOT_FOUND)

    # IF(NOT PYTHON_NOSETESTS_NOT_FOUND)
        # MESSAGE(STATUS "Searching for Python nosetests: ok")
    # ELSE()
        # MESSAGE(STATUS "Could NOT find Python nosetests ('import nose' failed)")
    # ENDIF()

    ######################################################################
    #
    #      check if sphinx documentation generator is installed
    #
    ######################################################################
    find_program(PYTHON_SPHINX sphinx-build "${PYTHON_PREFIX}/Scripts")

    IF(NOT PYTHON_SPHINX)
        MESSAGE(STATUS "Could NOT find sphinx documentation generator")
    ELSE()
        MESSAGE(STATUS "Searching for sphinx documentation generator: ok")
    ENDIF()

    ######################################################################
    #
    #      find Python platform
    #
    ######################################################################
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                     "import sys; p = sys.platform; print 'windows' if p.startswith('win') else p"
                      OUTPUT_VARIABLE PYTHON_PLATFORM OUTPUT_STRIP_TRAILING_WHITESPACE)

    ######################################################################
    #
    #      set outputs
    #
    ######################################################################
    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(PYTHONNODE_DEPENDENCIES DEFAULT_MSG
                         PYTHONINTERP_V2_FOUND PYTHONLIBS_FOUND
                         Boost_PYTHON_FOUND PYTHONNODE_INSTALL_DIR)

    IF(NOT PYTHONNODE_INCLUDE_DIRS OR PYTHONNODE_INCLUDE_DIRS MATCHES "-NOTFOUND")
        #note that the numpy include dir is set _before_ the python include dir, such that
        #installing a more recent version of numpy on top of an existing python installation
        #works (otherwise, numpy includes are picked up from ${PYTHON_INCLUDE_PATH}/numpy )
        SET(PYTHONNODE_INCLUDE_DIRS ${PYTHON_NUMPY_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH} ${Boost_INCLUDE_DIR})
    ENDIF()
    SET(PYTHONNODE_INCLUDE_DIRS ${PYTHONNODE_INCLUDE_DIRS}
        CACHE PATH "include directories needed by FRAPPER Python bindings"
        FORCE)
    IF(NOT PYTHONNODE_LIBRARIES OR PYTHONNODE_LIBRARIES MATCHES "-NOTFOUND")
        SET(PYTHONNODE_LIBRARIES ${PYTHON_LIBRARIES} ${Boost_PYTHON_LIBRARY})
    ENDIF()
    SET(PYTHONNODE_LIBRARIES ${PYTHONNODE_LIBRARIES}
        CACHE FILEPATH "libraries needed by FRAPPER Python bindings"
        FORCE)
		
	mark_as_advanced( FORCE 
		PYTHONNODE_INSTALL_DIR
		PYTHONNODE_INCLUDE_DIRS
		PYTHONNODE_LIBRARIES
		PYTHON_INCLUDE_PATH
		PYTHON_LIBRARIES
		PYTHON_SPHINX		
		)
ENDIF()

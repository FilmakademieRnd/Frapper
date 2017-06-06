project(ToFLib)

# ToFLib
set( res_header
	ToFLibNode.h
	ToFLibNodePlugin.h
	ToFLibRenderer.h
	)

set( res_moc
	ToFLibNode.h
	ToFLibNodePlugin.h
	)

set( res_source
	ToFLibNode.cpp
	ToFLibNodePlugin.cpp
	ToFLibRenderer.cpp
	)

set( res_description
	ToFLibNode.xml
	)

# Check ToFLib node dependencies..
SET( OXO_DIR ${THIRDPARTY_DIR}/oxofyuzit )
IF( IS_DIRECTORY ${OXO_DIR} )
	#---- OpenMP -------
	FIND_PACKAGE( OpenMP)
	if(OPENMP_FOUND)
		message("OPENMP FOUND")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	endif()	
	#---- OpenCV -------
	FIND_PACKAGE(OpenCV REQUIRED HINTS ${THIRDPARTY_DIR}/opencv )

	SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OXO_DIR}/config)

	#---- Vigra -------
	#FIND_PACKAGE(Vigra REQUIRED)
	
	#---- HDF5 -------
	FIND_PACKAGE(HDF5 REQUIRED)
	INCLUDE_DIRECTORIES( ${HDF5_INCLUDE_DIR} )

	#---- CImg -------
	FIND_PACKAGE(CImg REQUIRED HINTS ${OXO_DIR}/extern)

	#---- Ceres -------
	FIND_PACKAGE(CERES REQUIRED HINTS ${OXO_DIR}/extern/ceres)
	ADD_DEFINITIONS(-DUSE_CERES)

	#---- OpenGL -------
	FIND_PACKAGE(OPENGL)

	#---- GLFW -------
	FIND_PACKAGE(GLFW REQUIRED HINTS ${OXO_DIR}/extern/glfw-3.0.4/lib/cmake/glfw)
	ADD_DEFINITIONS(-DUSE_GLFW)

	#---- GLEW -------
	FIND_PACKAGE(GLEW)

	#---- CUDA -------
	FIND_PACKAGE(CUDA)

	IF( OpenCV_FOUND )

		# add additional include directories
		set( add_include_dir
			${OXO_DIR}/include
			${OXO_DIR}/extern
			${Vigra_INCLUDE_DIRS}
			${OpenCV_INCLUDE_DIRS}
			${GLFW_INCLUDE_DIR}
			${CUDA_TOOLKIT_INCLUDE}
			# ${CUDA_SDK_ROOT_DIR}/C/common/inc
			# ${CUDA_SDK_ROOT_DIR}/shared/inc
			${GLEW_INCLUDE_DIR}
			${HDF5_INCLUDE_DIR}
			${CERES_INCLUDE}
		)

		# Add additional library dependencies
		set( add_link_lib
			${OpenCV_LIBS}
			${VIGRA_LIBRARIES}
			${HDF5_CORE_LIBRARY}
			${HDF5_HL_LIBRARY}
			${GLFW_LIBRARY}
			${GLEW_LIBRARY} 
			${OPENGL_gl_LIBRARY}
			${OPENGL_glu_LIBRARY}
			${CERES_LIBRARIES}
		)
		
		set( add_link_dir
			${Vigra_LIBRARY_DIRS}
		)
		if( CUDA_FOUND )
			if( IS_DIRECTORY ${CUDA_SDK_ROOT_DIR})
				ADD_DEFINITIONS(-DHAS_CUDA)
				
				# if you get error messages in nvcc-generated files,
				# enable the following line for debugging:
				set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS};--keep")
				set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS};-Xcompiler;-fpermissive")
				
				INCLUDE_DIRECTORIES(${add_include_dir})
				INCLUDE_DIRECTORIES(${CUDA_TOOLKIT_INCLUDE})
				INCLUDE_DIRECTORIES(${CUDA_SDK_ROOT_DIR}/common/inc)
				INCLUDE_DIRECTORIES(${CUDA_SDK_ROOT_DIR}/C/common/inc)
				INCLUDE_DIRECTORIES(${CUDA_SDK_ROOT_DIR}/shared/inc)			
				INCLUDE_DIRECTORIES(${Vigra_INCLUDE_DIRS})			
				
				CUDA_ADD_LIBRARY( lib_tv_fusion ${OXO_DIR}/lib/tv_fusion/tv_fusion.cpp 
												${OXO_DIR}/lib/tv_fusion/tv_fusion.cu
												${OXO_DIR}/include/common/tv_fusion_impl.hxx )
												
				TARGET_LINK_LIBRARIES( lib_tv_fusion ${CUDA_CUTIL_LIBRARY} 
													 ${OpenCV_LIBS} 
													 ${VIGRA_LIBRARIES}
													 ${HDF5_CORE_LIBRARY} 
													 ${HDF5_HL_LIBRARY} 
													 ${GLFW_LIBRARY} 
													 ${GLEW_LIBRARY} 
													 ${OPENGL_gl_LIBRARY})
				
				set( add_link_lib ${add_link_lib} lib_tv_fusion )
				
				if( FRAPPER_SOLUTION_USE_FOLDERS AND VS_PROJECT_FOLDER )
					set_target_properties (lib_tv_fusion PROPERTIES FOLDER ${VS_PROJECT_FOLDER})	
				endif( FRAPPER_SOLUTION_USE_FOLDERS AND VS_PROJECT_FOLDER )
				
			else( IS_DIRECTORY ${CUDA_SDK_ROOT_DIR} )
				message( "CUDA SDK not found! Skipping TV Fusion." )
			endif( IS_DIRECTORY ${CUDA_SDK_ROOT_DIR} )
		else( CUDA_FOUND )
			message( "CUDA not found! Skipping TV Fusion." )
		endif( CUDA_FOUND )
		
		# additional resource files to install in frapper binary folder, e.g. /bin/win32 or /bin/x64
		set( res_bin
			${OXO_DIR}/include/common/depth_reprojection_fragment.glsl
			${OXO_DIR}/include/common/depth_reprojection_vertex.glsl
			${OXO_DIR}/include/common/depth_reprojection_geometry.glsl
			${OXO_DIR}/extern/glew-1.9.0/bin/glew32.dll
		)

		include( add_project )
	
	ENDIF() # OpenCV_FOUND
ENDIF() #IS_DIRECTORY ${OXO_DIR}
project(razerhydra)

# razerhydra
set( res_header
	RazerHydraNode.h
	RazerHydraNodePlugin.h
)

set( res_moc
	RazerHydraNode.h
	RazerHydraNodePlugin.h
)

set( res_source
	RazerHydraNode.cpp
	RazerHydraNodePlugin.cpp
)

set( res_description
	razerhydra.xml
)

set( res_additional
	razer-hydra.png
	)

# declare usage of sixenseSDK library
# TODO: Move this to a file called findHydra.cmake and use find_package
set( add_include_dir ${THIRDPARTY_DIR}/sixenseSDK/include )
IF( IS_X64)
	set( add_link_dir
		${THIRDPARTY_DIR}/sixenseSDK/lib/x64/release_dll
		${THIRDPARTY_DIR}/sixenseSDK/lib/x64/debug_dll
	)
	set( add_link_lib
		optimized sixense_x64 debug sixensed_x64
		optimized sixense_utils_x64 debug sixense_utilsd_x64
	)
ELSE( IS_X64)
	set( add_link_dir
		${THIRDPARTY_DIR}/sixenseSDK/lib/win32/release_dll
		${THIRDPARTY_DIR}/sixenseSDK/lib/win32/debug_dll
	)
	set( add_link_lib
		optimized sixense debug sixensed
		optimized sixense_utils debug sixense_utilsd
	)
ENDIF( IS_X64)

include( add_project )
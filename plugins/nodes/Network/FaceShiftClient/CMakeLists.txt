project(FaceShiftClient)

# header files
set( res_header
    FaceShiftClientNode.h
    FaceShiftClientNodePlugin.h
    fs_network/fsbinarystream.h
)

set( res_moc
	FaceShiftClientNode.h
	FaceShiftClientNodePlugin.h
)

set( res_source
	FaceShiftClientNode.cpp
	FaceShiftClientNodePlugin.cpp
    fs_network/fsbinarystream.cpp
)

set( res_description
	FaceShiftClient.xml
)

IF(FRAPPER_USE_QT5)
  set( add_link_lib
    ${QT_NETWORK}
  )
ELSE() 
  set( add_link_lib
    optimized QtNetwork4 debug QtNetworkd4
  )
ENDIF()

include( add_project )
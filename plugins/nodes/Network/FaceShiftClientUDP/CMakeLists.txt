project(FaceShiftClientUDP)

# header files
set( res_header
    FaceShiftClientUDPNode.h
    FaceShiftClientUDPNodePlugin.h
    fsstream.h
)

set( res_moc
	FaceShiftClientUDPNode.h
	FaceShiftClientUDPNodePlugin.h
    fsstream.h
)

set( res_source
	FaceShiftClientUDPNode.cpp
	FaceShiftClientUDPNodePlugin.cpp
    fsstream.cpp
)

set( res_description
	FaceShiftClientUDP.xml
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
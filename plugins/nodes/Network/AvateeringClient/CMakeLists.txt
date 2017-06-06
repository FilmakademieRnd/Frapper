project(AvateeringClient)

# header files
set( res_header
    AvateeringClientNode.h
    AvateeringClientNodePlugin.h
	DebugDrawer.h
)

set( res_moc
	AvateeringClientNode.h
	AvateeringClientNodePlugin.h
)

set( res_source
	AvateeringClientNode.cpp
	AvateeringClientNodePlugin.cpp
	DebugDrawer.cpp
)

set( res_description
	AvateeringClient.xml
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
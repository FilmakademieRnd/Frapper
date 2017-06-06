project(broadcast)

# broadcast
set( res_header
			BroadcastNode.h
			BroadcastNodePlugin.h
			)

set( res_moc
			BroadcastNode.h
			BroadcastNodePlugin.h
			)

set( res_source
			BroadcastNode.cpp
			BroadcastNodePlugin.cpp
			)

set( res_description
			broadcast.xml
			)

IF(FRAPPER_USE_QT5)
  set( add_link_lib
    ${QT_NETWORK}
  )
ELSE() 
  set( add_link_lib
    optimized QtMultimedia4 debug QtMultimediad4
    optimized QtNetwork4 debug QtNetworkd4
  )
ENDIF()


include( add_project )
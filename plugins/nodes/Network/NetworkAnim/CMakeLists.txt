project(networkanim)

# networkanim
set( res_header
			NetworkAnimNode.h
			NetworkAnimNodePlugin.h
			)

set( res_moc
			NetworkAnimNode.h
			NetworkAnimNodePlugin.h
			)

set( res_source
			NetworkAnimNode.cpp
			NetworkAnimNodePlugin.cpp
			)

set( res_description
			networkanim.xml
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
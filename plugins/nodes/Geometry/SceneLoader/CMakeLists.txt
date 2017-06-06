project(sceneLoader)

set( res_header
			SceneLoaderNode.h
			SceneLoaderNodePlugin.h
      DotsceneLoader.h
			tinystr.h
      tinyxml.h
			)

set( res_moc
			SceneLoaderNode.h
			SceneLoaderNodePlugin.h
			)

set( res_source
			SceneLoaderNode.cpp
			SceneLoaderNodePlugin.cpp
      DotSceneLoader.cpp
      tinyxml.cpp
      tinystr.cpp
      tinyxmlerror.cpp
      tinyxmlparser.cpp
			)

set( res_description
	sceneLoader.xml
)

include( add_project )
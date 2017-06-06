project(S3DGame)

# s3dgame
set( res_header
	S3DGame.h
	S3DGameEngine.h
	S3DGamePaths.h
	S3DGameNode.h
	S3DGameNodePlugin.h
	)

set( res_moc
	S3DGame.h
	S3DGameNode.h
	S3DGameNodePlugin.h
	)

set( res_source
	S3DGame.cpp
	S3DGameEngine.cpp
	S3DGameNode.cpp
	S3DGameNodePlugin.cpp
	)

set( res_description
     s3dgame.xml
	 )

include( add_project )
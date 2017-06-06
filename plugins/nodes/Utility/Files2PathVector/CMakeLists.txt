project(files2PathVector)

# timer
set( res_header
			Files2PathVectorNode.h
			Files2PathVectorNodePlugin.h
			)

set( res_moc
			Files2PathVectorNode.h
			Files2PathVectorNodePlugin.h
			)

set( res_source
			Files2PathVectorNode.cpp
			Files2PathVectorNodePlugin.cpp
			)

set( res_description
			Files2PathVector.xml
			)
		
include( add_project )
project(imagefile)

# imagefile
set( res_header
			ImageFileNode.h
			ImageFileNodePlugin.h
			)

set( res_moc
			ImageFileNode.h
			ImageFileNodePlugin.h
			)

set( res_source
			ImageFileNode.cpp
			ImageFileNodePlugin.cpp
			)
			
set( res_description
			imagefile.xml
			)

include( add_project )
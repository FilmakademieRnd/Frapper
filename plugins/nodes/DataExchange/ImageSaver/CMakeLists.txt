project(imagesaver)

# imagesaver
set( res_header
			ImageSaverNode.h
			ImageSaverNodePlugin.h
			)

set( res_moc
			ImageSaverNode.h
			ImageSaverNodePlugin.h
			)

set( res_source
			ImageSaverNode.cpp
			ImageSaverNodePlugin.cpp
			)

set( res_description
			imagesaver.xml
			)

include( add_project )
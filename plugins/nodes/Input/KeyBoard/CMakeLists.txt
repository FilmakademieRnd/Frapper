project(keyboard)

# Keyboard
set( res_header
			KeyboardNode.h
			KeyboardNodePlugin.h
			)

set( res_moc
			KeyboardNode.h
			KeyboardNodePlugin.h
			)

set( res_source
			KeyboardNode.cpp
			KeyboardNodePlugin.cpp
			)

set( res_description
			Keyboard.xml
			)

include( add_project )
project(miditimer)

# timer
set( res_header
			MidiTimerNode.h
			MidiTimerNodePlugin.h
			RtMidi.h
			)

set( res_moc
			MidiTimerNode.h
			MidiTimerNodePlugin.h
			RtMidi.h
			)

set( res_source
			MidiTimerNode.cpp
			MidiTimerNodePlugin.cpp
			RtMidi.cpp
			)

set( res_description
			miditimer.xml
			)
			
set( add_link_lib
		winmm)
		
add_definitions( -D__WINDOWS_MM__ )
		
include( add_project )
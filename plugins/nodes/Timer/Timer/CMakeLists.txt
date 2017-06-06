project(timer)

# timer
set( res_header
			timerNode.h
			timerNodePlugin.h
			)

set( res_moc
			timerNode.h
			timerNodePlugin.h
			)

set( res_source
			timerNode.cpp
			timerNodePlugin.cpp
			)

set( res_description
			timer.xml
			)

include( add_project )
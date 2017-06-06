project(trigger)

# trigger
set( res_header
			TriggerNode.h
			TriggerNodePlugin.h
			)

set( res_moc
			TriggerNode.h
			TriggerNodePlugin.h
			)

set( res_source
			TriggerNode.cpp
			TriggerNodePlugin.cpp
			)

set( res_description
			trigger.xml
			)

include( add_project )
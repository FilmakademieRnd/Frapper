project(PointMover)

# PointMover
set( res_header			
			PointMoverNode.h
			PointMoverNodePlugin.h
			)

set( res_moc
			PointMoverNode.h
			PointMoverNodePlugin.h
			)

set( res_source
			PointMoverNode.cpp
			PointMoverNodePlugin.cpp
			)

set( res_description
			PointMoverNode.xml
			)

include( add_project )
project(wiitracker)

# wiitracker
set( res_header
			WiiTrackerNode.h
			WiiTrackerNodePlugin.h
			)

set( res_moc
			WiiTrackerNode.h
			WiiTrackerNodePlugin.h
			)

set( res_source
			WiiTrackerNode.cpp
			WiiTrackerNodePlugin.cpp
			)

set( res_description
			wiitracker.xml
			)

# declare linkage of wiiuse library
set( add_include_dir ${THIRDPARTY_DIR}/wiiuse/include )
set( add_link_dir ${THIRDPARTY_DIR}/wiiuse/lib )
set( add_link_lib wiiuse )

include( add_project )
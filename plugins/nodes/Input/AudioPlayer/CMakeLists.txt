project(AudioPlayer)

# camera
set( res_header
			AudioPlayerNode.h
			AudioPlayerNodePlugin.h
			)

set( res_moc
			AudioPlayerNode.h
			AudioPlayerNodePlugin.h
			)

set( res_source
			AudioPlayerNode.cpp
			AudioPlayerNodePlugin.cpp
			)

set( res_description
			audioplayer.xml
			)

# declare linkage of bass library
set( add_include_dir ${THIRDPARTY_DIR}/bass/include )
set( add_link_dir ${THIRDPARTY_DIR}/bass/lib )
set( add_link_lib bass )

include( add_project )
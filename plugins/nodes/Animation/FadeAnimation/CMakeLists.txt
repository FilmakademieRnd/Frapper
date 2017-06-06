project(fadeAnim)

# fadeAnim
set( res_header
    FadeAnimNode.h
    FadeAnimNodePlugin.h
)

set( res_moc
    FadeAnimNode.h
    FadeAnimNodePlugin.h
)

set( res_source
    FadeAnimNode.cpp
    FadeAnimNodePlugin.cpp
)

set( res_description
    fadeAnim.xml
)

include( add_project )
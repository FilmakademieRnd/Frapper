project(animationclip)

# animationclip
set( res_header
    AnimationClipNode.h
    AnimationClipNodePlugin.h
)

set( res_moc
    AnimationClipNode.h
    AnimationClipNodePlugin.h
)

set( res_source
    AnimationClipNode.cpp
    AnimationClipNodePlugin.cpp
)

set( res_description
    animationclip.xml
)

include( add_project )
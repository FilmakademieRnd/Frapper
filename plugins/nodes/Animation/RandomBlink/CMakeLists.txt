project(randomblink)

# randomblink
set( res_header
    RandomblinkNode.h
    RandomblinkNodePlugin.h
)

set( res_moc
    RandomblinkNode.h
    RandomblinkNodePlugin.h
)

set( res_source
    RandomblinkNode.cpp
    RandomblinkNodePlugin.cpp
)

set( res_description
    randomblink.xml
)

include( add_project )
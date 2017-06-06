project(surfacecurvature)

set( res_header
    SurfaceCurvatureNode.h
    SurfaceCurvatureNodePlugin.h
)

set( res_moc
    SurfaceCurvatureNode.h
    SurfaceCurvatureNodePlugin.h
)

set( res_source
    SurfaceCurvatureNode.cpp
    SurfaceCurvatureNodePlugin.cpp
)

set( res_description
    surfacecurvature.xml
)

include( add_project )
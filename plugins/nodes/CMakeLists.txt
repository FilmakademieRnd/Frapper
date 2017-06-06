project(nodes)

set( Folders
  Animation
  Cameras
  Compositing
  DataExchange
  Geometry
  Images
  Input
  Lights
  Network
  NPR
  Rendering
  S3D
  Shading
  Timer
  Tracking
  Utility
)
  
foreach( folder ${Folders} )
  set( VS_PROJECT_FOLDER "Nodes/${folder}" )
  add_subdirectory( ${folder})
endforeach()
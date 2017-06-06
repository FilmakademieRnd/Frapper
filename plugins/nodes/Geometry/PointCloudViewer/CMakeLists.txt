project(pointcloudviewer)

# pointcloudviewer
set( res_header			
			PointCloudViewerNode.h
			PointCloudViewerNodePlugin.h
			)

set( res_moc
			PointCloudViewerNode.h
			PointCloudViewerNodePlugin.h
			)

set( res_source
			PointCloudViewerNode.cpp
			PointCloudViewerNodePlugin.cpp
			)

set( res_description
			pointcloudviewer.xml
			)

include( add_project )
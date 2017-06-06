project(PointCloudReader)

# pointcloudreader
set( res_header 		
	PointCloudReaderNode.h
	PointCloudReaderNodePlugin.h
	)

set( res_moc 	
	PointCloudReaderNode.h
	PointCloudReaderNodePlugin.h
	)

set( res_source 	
	PointCloudReaderNode.cpp
	PointCloudReaderNodePlugin.cpp
	)

set( res_description 	
	pointcloudreader.xml
	)

include( add_project )

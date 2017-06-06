project(image2VertexBuffer)
			
set( res_header
			image2VertexBufferNode.h
			image2VertexBufferNodePlugin.h
			)

set( res_moc
			image2VertexBufferNode.h
			image2VertexBufferNodePlugin.h
			)

set( res_source
			image2VertexBufferNode.cpp
			image2VertexBufferNodePlugin.cpp
			)

set( res_description
			image2vertexbuffer.xml
			)

include( add_project )
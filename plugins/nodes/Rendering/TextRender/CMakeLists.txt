project(textRender)

# TextRender
set( res_header
			TextRenderNode.h
			TextRenderPlugin.h
			)

set( res_moc
			TextRenderNode.h
			TextRenderPlugin.h
			)

set( res_source
			TextRenderNode.cpp
			TextRenderPlugin.cpp
			)
			
set( res_description
			textRender.xml
			)

include( add_project )
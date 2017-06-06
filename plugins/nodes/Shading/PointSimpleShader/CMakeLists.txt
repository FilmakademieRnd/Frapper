project(PointSimpleShader)

# PointSimpleShader
set( res_header			
			PointSimpleShaderNode.h
			PointSimpleShaderNodePlugin.h
			)

set( res_moc
			PointSimpleShaderNode.h
			PointSimpleShaderNodePlugin.h
			)

set( res_source
			PointSimpleShaderNode.cpp
			PointSimpleShaderNodePlugin.cpp
			)

set( res_description
			PointSimpleShaderNode.xml
			)

include( add_project )
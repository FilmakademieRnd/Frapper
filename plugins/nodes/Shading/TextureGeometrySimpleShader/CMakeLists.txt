project(PointSimpleShader)

# PointSimpleShader
set( res_header			
			TextureGeometrySimpleShaderNode.h
			TextureGeometrySimpleShaderNodePlugin.h
			)

set( res_moc
			TextureGeometrySimpleShaderNode.h
			TextureGeometrySimpleShaderNodePlugin.h
			)

set( res_source
			TextureGeometrySimpleShaderNode.cpp
			TextureGeometrySimpleShaderNodePlugin.cpp
			)

set( res_description
			TextureGeometrySimpleShaderNode.xml
			)

include( add_project )
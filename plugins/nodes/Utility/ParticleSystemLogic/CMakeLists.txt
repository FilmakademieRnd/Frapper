project(particlesystemlogic)

# ExpressionTimer
set( res_header
			ParticleSystemLogicNode.h
			ParticleSystemLogicNodePlugin.h			
			)

set( res_moc
			ParticleSystemLogicNode.h
			ParticleSystemLogicNodePlugin.h			
			)
set( res_source
			ParticleSystemLogicNode.cpp
			ParticleSystemLogicNodePlugin.cpp
			)

set( res_description
			particlesystemlogic.xml
			)
            

include( add_project )
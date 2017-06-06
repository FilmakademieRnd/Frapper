project(particlesystem)

set (	INCLUDE_DIRS
		partikkle/include
		glm
		)

include_directories(
	${INCLUDE_DIRS}
	)

# particlesystem
set( res_header
			ParticleSystemNode.h
			ParticleSystemNodePlugin.h
			partikkle/include/AxisAffector.h
			partikkle/include/Partikkle.h
			partikkle/include/InterParticleAffector.h
			partikkle/include/KDTree.h
			partikkle/include/KDTreeNode.h
			partikkle/include/MeshAffector.h
			partikkle/include/ParticleAffector.h
			partikkle/include/ParticleEmitter.h
			partikkle/include/ParticleSystem.h
			partikkle/include/PlaneAffector.h
			partikkle/include/PointEmitter.h
			partikkle/include/PointAffector.h
			partikkle/include/RandomEmitter.h
			partikkle/include/SpatialHashTable.h
			)

set( res_moc
			ParticleSystemNode.h
			ParticleSystemNodePlugin.h
			)

set( res_source
			ParticleSystemNode.cpp
			ParticleSystemNodePlugin.cpp
			partikkle/src/AxisAffector.cpp
			partikkle/src/InterParticleAffector.cpp
			partikkle/src/KDTree.cpp
			partikkle/src/KDTreeNode.cpp
			partikkle/src/MeshAffector.cpp
			partikkle/src/ParticleAffector.cpp
			partikkle/src/ParticleEmitter.cpp
			partikkle/src/ParticleSystem.cpp
			partikkle/src/PlaneAffector.cpp
			partikkle/src/PointEmitter.cpp
			partikkle/src/PointAffector.cpp
			partikkle/src/RandomEmitter.cpp
			partikkle/src/SpatialHashTable.cpp
			)
			
set( res_description
			particlesystem.xml
			)

include( add_project )

# Add additional library dependencies
IF(FRAPPER_USE_QT5)
  set( add_link_lib
    ${QT_CONCURRENT}
  )
ENDIF()
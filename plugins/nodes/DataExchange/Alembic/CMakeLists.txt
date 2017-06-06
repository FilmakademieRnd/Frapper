project(alembicexport)

set( res_header 		
	AlembicExportNode.h
	AlembicExportNodePlugin.h
	)

set( res_moc 	
	AlembicExportNode.h
	AlembicExportNodePlugin.h
	)

set( res_source 	
	AlembicExportNode.cpp
	AlembicExportNodePlugin.cpp
	)

set( res_description 	
	alembicexport.xml
	)

# declare linkage of alembic library
set( add_include_dir 
	${THIRDPARTY_DIR}/alembic/ilmbase/include
	${THIRDPARTY_DIR}/alembic/alembic/include
	${THIRDPARTY_DIR}/alembic/hdf5/include
)

set( add_link_dir
	${THIRDPARTY_DIR}/alembic/ilmbase/lib
	${THIRDPARTY_DIR}/alembic/alembic/lib/static
	${THIRDPARTY_DIR}/alembic/hdf5/lib
)

set( add_link_lib
	hdf5
	hdf5_hl
	Half
	AlembicUtil
	AlembicAbc
	AlembicAbcCoreAbstract
	AlembicAbcCoreHDF5
	AlembicAbcGeom
)

include( add_project )
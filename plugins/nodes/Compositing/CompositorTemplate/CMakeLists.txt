project(compositortemplate)

# compositortemplate
set( res_header
    CompositorTemplateNode.h
    CompositorTemplateNodePlugin.h
    )

set( res_moc
    CompositorTemplateNode.h
    CompositorTemplateNodePlugin.h
    )

set( res_source
    CompositorTemplateNode.cpp
    CompositorTemplateNodePlugin.cpp
    )

set( res_description
    bloom.xml
    boxblur.xml
	brightnessContrast.xml
    colorkey.xml
	depth2stereo.xml
    depthoffield.xml
	freichen.xml
    gaussblur.xml
    lensedistortion.xml
    mask.xml
	multiblend.xml
    objectidfilter.xml
    skin.xml
	sobel.xml
	cld.xml
	imageabstraction.xml
	pencildrawing.xml
	lns.xml
	voronoi.xml
	oilpainting.xml
)

include( add_project )
/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; version 2.1 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
-----------------------------------------------------------------------------
*/

//!
//! \file "ViewportWidget.cpp"
//! \brief Implementation file for ViewportWidget class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0f
//! \date       29.06.2009 (last updated)
//!

#include "ViewportWidget.h"
#include "OgreTools.h"
#include "OgreManager.h"
#include "OgreFontManager.h"
#include "Log.h"
#include <QApplication>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#if !defined(Q_OS_WIN)
#include <QX11Info>
#endif


namespace Frapper {

INIT_INSTANCE_COUNTER(ViewportWidget)


///
/// Private Static Data
///


//!
//! A simple Default vertex shader
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultVertexShader;

//!
//! A simple lighting vertex shader
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultLitVertexShader;

//!
//! A simple Default fragment shader
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultFragmentShader;

//!
//! A simple lighting fragment shader
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultLitFragmentShader;

//!
//! A simple Default vertex shader with UV-Coordinates
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultUVVertexShader;

//!
//! The vertex shader for text drawing
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultFontVertexShader;

//!
//! A simple Default fragment shader with one textue
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultTU0FragmentShader;

//!
//!	The Fragment shader for text drawing
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::s_defaultFontFragmentShader;

//!
//! Material for self-illuminating red objects (like coordinate X axes).
//!
Ogre::MaterialPtr ViewportWidget::s_redMaterial;

//!
//! Material for self-illuminating green objects (like coordinate Y axes).
//!
Ogre::MaterialPtr ViewportWidget::s_greenMaterial;

//!
//! Material for self-illuminating blue objects (like coordinate Z axes).
//!
Ogre::MaterialPtr ViewportWidget::s_blueMaterial;

//!
//! Material for self-illuminating gray objects.
//!
Ogre::MaterialPtr ViewportWidget::s_grayMaterial;

//!
//! Material for self-illuminating black objects.
//!
Ogre::MaterialPtr ViewportWidget::s_blackMaterial;

//!
//! Default material to use in shaded mode.
//!
Ogre::MaterialPtr ViewportWidget::s_defaultMaterial;

//!
//! Material for Null objects.
//!
Ogre::MaterialPtr ViewportWidget::s_nullMaterial;


///
/// Constructors and Destructors
///


//!
//! Constructor of the ViewportWidget class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param stageIndex The index of the stage to display in the viewport.
//! \param fullscreen Flag that controls whether the widget is used in fullscreen mode.
//! \param width The width of the viewport when in fullscreen mode.
//! \param height The height of the viewport when in fullscreen mode.
//!
ViewportWidget::ViewportWidget ( QWidget *parent, unsigned int stageIndex /* = 1 */, bool fullscreen /* = false */, int width /* = 1024 */, int height /* = 768 */ ) :
    QWidget(parent),
    m_stageIndex(stageIndex),
	m_viewportNbr(s_totalNumberOfInstances),
    m_fullscreen(fullscreen),
    m_width(width),
    m_height(height),
    // OGRE system objects
    m_renderWindow(0),
    m_sceneManager(OgreManager::getSceneManager()),
    m_cameraSceneNode(0),
    m_camera(0),
    m_cameraName(""),
    m_headLight(0),
    m_viewport(0),
    m_backgroundColor(Ogre::ColourValue(0.33f, 0.33f, 0.33f)),
    m_orientationIndicatorCameraSceneNode(0),
    m_orientationIndicatorCamera(0),
    m_orientationIndicatorViewport(0),
    // Overlays
    m_labelOverlay(0),
    m_imageOverlay(0),
    m_leftTopHudPanel(0),
    m_leftTopHudTextArea(0),
    m_imagePanel(0),
    // OGRE scene nodes
    m_viewportObjectsSceneNode(0),
    m_originSceneNode(0),
    m_centerPointSceneNode(0),
    m_orientationIndicatorSceneNode(0),
    m_gridSceneNode(0),
    // viewing flags
    m_viewMode(false),
    m_lastPosition(QPoint(0, 0)),
    m_viewingParameters(new ViewingParameters()),
    // display flags
    m_wireframe(false),
    m_backfaceCulling(false),
    m_showOrigin(false),
    m_showCenterPoint(false),
    m_showOrientationIndicator(false),
    m_showGrid(false),
    m_overlayOnly(false),
    m_dbgColor(0.0f)
{
    // set Qt widget properties
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // create the render window
    Log::debug("Creating render window...", "ViewportWidget::ViewportWidget");
    bool renderWindowCreated = createRenderWindow();
    setAttribute(Qt::WA_PaintOnScreen, m_sceneManager && renderWindowCreated);
    setAttribute(Qt::WA_NoSystemBackground, m_sceneManager && renderWindowCreated);
    setAttribute(Qt::WA_OpaquePaintEvent, m_sceneManager && renderWindowCreated);
    if (renderWindowCreated) {
        Log::debug("Initializing resources...", "ViewportWidget::ViewportWidget");
        initializeResources();
        Log::debug("Creating viewport objects...", "ViewportWidget::ViewportWidget");
        createViewportObjects();
        Log::debug("Creating overlays...", "ViewportWidget::ViewportWidget");
        createOverlays();

        homeView();
        setWireframe(m_wireframe);
        setBackfaceCulling(m_backfaceCulling);
    }

	// setup timer for caption framerate update
    m_timer = new QTimer(); 
    connect(m_timer, SIGNAL(timeout()), SLOT(updateTimer()));
	m_timer->setInterval(3000);
    m_timer->start();

    INC_INSTANCE_COUNTER
}


//!
//! Destructor of the ViewportWidget class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
ViewportWidget::~ViewportWidget ()
{
	m_timer->stop();
	delete m_viewingParameters;

	Ogre::MaterialManager *materialManager = Ogre::MaterialManager::getSingletonPtr();

	if (!m_imageMaterial.isNull())
		materialManager->remove(m_imageMaterial->getName());

	destroyOverlays();

	//m_renderWindow->removeAllListeners();
	//m_renderWindow->removeAllViewports();

	//Ogre::Root *ogreRoot = OgreManager::getRoot();
	//if (ogreRoot) {
	//	Ogre::RenderSystem *renderSystem = ogreRoot->getRenderSystem();
	//	if (renderSystem)
	//		renderSystem->destroyRenderWindow(m_renderWindow->getName());
	//}

	OgreTools::deepDeleteSceneNode(m_viewportObjectsSceneNode, m_sceneManager, true);

	DEC_INSTANCE_COUNTER

	if (s_numberOfInstances == 0) {
		s_redMaterial.setNull();
		s_greenMaterial.setNull();
		s_blueMaterial.setNull();
		s_grayMaterial.setNull();
		s_blackMaterial.setNull();
		s_defaultMaterial.setNull();
		s_nullMaterial.setNull();
		m_imageMaterial.setNull();

		s_defaultVertexShader.setNull();
		s_defaultLitVertexShader.setNull();
		s_defaultFragmentShader.setNull();
		s_defaultLitFragmentShader.setNull();
		s_defaultUVVertexShader.setNull();
		s_defaultTU0FragmentShader.setNull();
		s_defaultFontVertexShader.setNull();
		s_defaultFontFragmentShader.setNull();
	}
}


///
/// Public Functions
///


//!
//! Returns a pointer to the paint engine used for drawing on the device.
//!
//! Returning 0 by this function fixes the widget flickering bug in Qt 4.5.
//!
//! \return 0.
//!
QPaintEngine * ViewportWidget::paintEngine () const
{
    return 0;
}


//!
//! Returns the index of the stage to display in the viewport.
//!
//! \return The index of the stage to display in the viewport.
//!
unsigned int ViewportWidget::getStageIndex () const
{
    return m_stageIndex;
}


//!
//! Sets the index of the stage to display in the viewport.
//!
//! \param stageIndex The index of the new stage to display in the viewport.
//!
void ViewportWidget::setStageIndex ( unsigned int stageIndex )
{
	if (m_stageIndex != stageIndex) {
		destroyOverlays ();
		m_stageIndex = stageIndex;
		createOverlays ();
	}
	render();
}


//!
//! Activates or deactivates the view mode in the viewport widget.
//!
//! \param viewMode The new value for the view mode flag.
//!
void ViewportWidget::setViewMode ( bool viewMode )
{
    m_viewMode = viewMode;

    if (m_viewMode)
        setCursor(Qt::OpenHandCursor);
    else
        setCursor(Qt::ArrowCursor);
}


//!
//! Centers the viewport's view on currently selected objects.
//!
void ViewportWidget::frameSelectedObjects ()
{
    emit frameSelectedObjectsRequested(m_viewingParameters);

    updateView();
    render();
}

//!
//! Sets the fog parameters of the viewport.
//!
//! \param enabled Sets whether fog is enabled or not.
//! \param density Sets the density of the fog (exponential).
//!
void ViewportWidget::setFog ( bool enabled, float density )
{
    if (m_sceneManager) {
        if (enabled)
            m_sceneManager->setFog(Ogre::FOG_EXP, m_fogColor, density);
        else
            m_sceneManager->setFog();
        updateView();
        render();
    }
}

//!
//! Returns the fog color of the scene.
//!
//! \return Fog color of the scene.
//!
QColor ViewportWidget::getFogColor () const
{
    QColor fogColor;
    fogColor.setRedF(m_fogColor.r);
    fogColor.setGreenF(m_fogColor.g);
    fogColor.setBlueF(m_fogColor.b);
    fogColor.setAlphaF(m_fogColor.a);
    return fogColor;
}

//!
//! Sets the fog color of the scene.
//!
//! \param color Fog color of the scene.
//!
void ViewportWidget::setFogColor ( const QColor &color )
{
    m_fogColor.r = color.redF();
    m_fogColor.g = color.greenF();
    m_fogColor.b = color.blueF();
    m_fogColor.a = color.alphaF();
}

//!
//! Returns the background color of the scene.
//!
//! \return Fog background of the scene.
//!
QColor ViewportWidget::getBackgroundColor () const
{
    QColor backgroundColor;
    backgroundColor.setRedF(m_backgroundColor.r);
    backgroundColor.setGreenF(m_backgroundColor.g);
    backgroundColor.setBlueF(m_backgroundColor.b);
    backgroundColor.setAlphaF(m_backgroundColor.a);
    return backgroundColor;
}

//!
//! Sets the background color of the scene.
//!
//! \param color Background color of the scene.
//!
void ViewportWidget::setBackgroundColor ( const QColor &color )
{
    m_backgroundColor.r = color.redF();
    m_backgroundColor.g = color.greenF();
    m_backgroundColor.b = color.blueF();
    m_backgroundColor.a = color.alphaF();
    if (m_viewport)
        m_viewport->setBackgroundColour(m_backgroundColor);
    render();
}

//!
//! Sets fullscreen mode for the viewport widget.
//!
//! \param fullscreen Flag to control whether to enable fullscreen mode.
//! \param width The width of the viewport in fullscreen mode.
//! \param height The height of the viewport in fullscreen mode.
//!
void ViewportWidget::setFullscreen ( bool fullscreen, int width, int height )
{
    if (m_renderWindow) {
		if (fullscreen) {
			m_labelOverlay->hide();
		}
		else
			m_labelOverlay->show();
        //m_renderWindow->setFullscreen(fullscreen, width, height);
	}
}


//!
//! Sets whether only overlays (no geometry) should be displayed in viewport.
//!
//! \param enable If true no geometry will be displayed.
//!
void ViewportWidget::setOverlayOnly ( bool enable )
{
    m_overlayOnly = enable;
}

//!
//! Returns the current number of frames per second rendered in the
//! viewport.
//!
//! \return The current number of frames per second rendered in the viewport.
//!
float ViewportWidget::getFps () const
{
    if (m_renderWindow)
        return m_renderWindow->getLastFPS();
    else
        return 0;
}


//!
//! Returns the number of triangles last rendered in the viewport.
//!
//! \return The number of triangles last rendered in the viewport.
//!
int ViewportWidget::getTriangleCount () const
{
    if (m_renderWindow)
        return (int) m_renderWindow->getTriangleCount();
    else
        return 0;
}


//!
//! Returns the viewing parameter's of the currently active camera.
//!
//! \return The viewing parameter's of the currently active camera.
//!
ViewingParameters * ViewportWidget::getViewingParameters () const
{
    return m_viewingParameters;
}


///
/// Public Slots
///


Ogre::RenderWindow* ViewportWidget::getRenderWindow() const
{
    return m_renderWindow;
}

//!
//! Applies the given viewing parameters to the viewport's viewing
//! parameters.
//!
//! \param viewingParameters The viewing parameters to apply to the viewport.
//!
void ViewportWidget::applyViewingParameters ( ViewingParameters *viewingParameters )
{
    viewingParameters->applyTo(m_viewingParameters);
}


//!
//! Sets the image that should be displayed in the viewport.
//!
//! Only affects viewports that are set to display the content of the stage
//! with the given index.
//!
//! \param stageIndex The index of the stage to which the image belongs.
//! \param image The image that should be displayed in the viewport.
//!
void ViewportWidget::setImage ( unsigned int stageIndex, Ogre::TexturePtr image )
{
    // discard the event if the viewport is set to a different stage
    if (stageIndex != m_stageIndex)
        return;

    if (!image.isNull()) {
        // use the image as a texture in the image overlay material
        Ogre::Pass *technique0pass0 = m_imageMaterial->getTechnique(0)->getPass(0);
        Ogre::TextureUnitState *textureUnitState0 = technique0pass0->getTextureUnitState(0);
        textureUnitState0->setTextureName(image->getName());

        // get image info
        unsigned int imageWidth = (unsigned int) image->getWidth();
        unsigned int imageHeight = (unsigned int) image->getHeight();
        QString pixelFormatName = QString("%1").arg(Ogre::PixelUtil::getFormatName(image->getFormat()).c_str());

        // update the image panel
        if (m_imagePanel) 
            m_imagePanel->setDimensions(imageWidth, imageHeight);
        
        // show the overlays
        m_showOverlays = true;
        if (m_imageOverlay)
            m_imageOverlay->show();

        // update the text information displayed in the HUD
		m_imageInfo = QString("Viewport\nImage: %1x%2 pixel\nPixel format: %3\nFPS: ").arg(imageWidth).arg(imageHeight).arg(pixelFormatName);
    } else {
        // hide the overlays
        m_showOverlays = false;
        if (m_imageOverlay)
            m_imageOverlay->hide();

        // update the text information displayed in the HUD
		m_imageInfo = QString("Viewport\nFPS: ");
    }
	
	if( m_leftTopHudTextArea)
		m_leftTopHudTextArea->setCaption( (m_imageInfo + QString::number((int) m_renderWindow->getAverageFPS())).toStdString() );
    // force a resize event to update the panel overlays' positions and dimensions
    resizeEvent(new QResizeEvent(size(), size()));
}


//!
//! Resets the viewport's view settings.
//!
void ViewportWidget::homeView ()
{
    m_viewingParameters->reset();
    updateView();
    render();
}


//!
//! Activates or deactivates wireframe view in the viewport.
//!
//! \param wireframe Flag that controls wireframe view in the viewport.
//!
void ViewportWidget::setWireframe ( bool wireframe )
{
    m_wireframe = wireframe;

    // set the Ogre polygon mode according to the flag's value
    Ogre::PolygonMode polygonMode;
    if (m_wireframe)
        polygonMode = Ogre::PM_WIREFRAME;
    else
        polygonMode = Ogre::PM_SOLID;

    // update the viewport's camera
    if (m_camera)
        m_camera->setPolygonMode(polygonMode);

    render();
}


//!
//! Activates or deactivates the omission of polygons facing away from the
//! camera.
//!
//! \param backfaceCulling Flag that controls the culling of polygons facing away from the camera.
//!
void ViewportWidget::setBackfaceCulling ( bool backfaceCulling )
{
    m_backfaceCulling = backfaceCulling;

    // set the Ogre culling mode according to the flag's value
    Ogre::CullingMode cullingMode;
    if (m_backfaceCulling)
        cullingMode = Ogre::CULL_CLOCKWISE;
    else
        cullingMode = Ogre::CULL_NONE;

    // iterate over all materials managed by the material manager
    Ogre::MaterialManager *materialManager = Ogre::MaterialManager::getSingletonPtr();
    if (materialManager) {
        Ogre::ResourceManager::ResourceMapIterator it = materialManager->getResourceIterator();
        while (it.hasMoreElements()) {
			Ogre::MaterialPtr currentMaterial = it.getNext().staticCast<Ogre::Material>();
            // set the culling mode for the material
            if (!currentMaterial.isNull() && currentMaterial->getGroup() != "ViewportMaterials") {
				Ogre::Technique *technique0 = currentMaterial->getTechnique(0);
				if (technique0 && technique0->getNumPasses() > 0) {
					technique0->getPass(0)->setCullingMode(cullingMode);
				}
			}
        }
    }

    render();
}


//!
//! Toggles the display of the point of origin in the viewport.
//!
void ViewportWidget::toggleOrigin ()
{
    m_showOrigin = !m_showOrigin;
    render();
}


//!
//! Toggles the display of the center point in the viewport.
//!
void ViewportWidget::toggleCenterPoint ()
{
    m_showCenterPoint = !m_showCenterPoint;
    render();
}


//!
//! Toggles the display of the orientation indicator in the viewport.
//!
void ViewportWidget::toggleOrientationIndicator ()
{
    m_showOrientationIndicator = !m_showOrientationIndicator;
    render();
}


//!
//! Toggles the display of the coordinate grid in the viewport.
//!
void ViewportWidget::toggleGrid ()
{
    m_showGrid = !m_showGrid;
    render();
}
//!
//! Write the contents of the current render window to image file
//!
void ViewportWidget::screenCapture ()
{
    m_renderWindow->writeContentsToFile("capture.png");
    Log::info("Created screen capture (\"capture.png\") in application root folder.", "ViewportWidget::screenCapture");
    render();
}

///
/// Protected Events
///


//!
//! Handles resize events for the widget.
//!
//! \param event The description of the resize event.
//!
void ViewportWidget::resizeEvent ( QResizeEvent *event )
{
    if (!m_renderWindow)
        return;

    int width = event->size().width();
    int height = event->size().height();

	if( width < 1) width = 1;
	if( height < 1) height = 1;

    // store the size of the widget in the viewing parameters
    m_viewingParameters->setCanvasSize(width, height);

#if defined(Q_WS_X11)
    m_renderWindow->resize(width, height);
#endif
    m_renderWindow->windowMovedOrResized();

    // set the camera aspect ratio to match the viewport
    if (m_camera)
        m_camera->setAspectRatio((Ogre::Real) width / height);

    // update the position and size of the orientation indicator viewport
    if (m_orientationIndicatorViewport)
        m_orientationIndicatorViewport->setDimensions(0, 1.0f - 40.0f / height, 40.0f / width, 40.0f / height);

    if (m_imageOverlay && m_imageOverlay->isVisible()) {
        // update image panel
        Ogre::Real imageWidth = m_imagePanel->getWidth();
        Ogre::Real imageHeight = m_imagePanel->getHeight();
        m_imagePanel->setPosition(-imageWidth / 2, -imageHeight / 2);
    }

    render();
}


//!
//! Handles paint events for the widget.
//!
//! \param event The description of the paint event.
//!
void ViewportWidget::paintEvent ( QPaintEvent *event )
{
    Q_UNUSED(event)

    //render();
}


//!
//! Handles mouse press events for the widget.
//!
//! \param event The description of the mouse event.
//!
void ViewportWidget::mousePressEvent ( QMouseEvent *event )
{
    m_lastPosition = event->pos();

    if (event->button() == Qt::MidButton)
        setCursor(Qt::SizeAllCursor);
    else
        if (m_viewMode) {
            if (event->button() == Qt::LeftButton)
                setCursor(Qt::ClosedHandCursor);
            else if (event->button() == Qt::RightButton)
                setCursor(Qt::SizeFDiagCursor);
        } else {
            if (event->button() == Qt::LeftButton) {
#if QT_VERSION >= 0x050000
				QPointF localPos = event->localPos();
#else
				QPointF localPos = event->posF();
#endif
                Ogre::SceneNode *clickedSceneNode = getClickedSceneNode(localPos);
				std::cout << "Viewport Widget x:" << localPos.rx() << " y:" << localPos.ry() << std::endl;

                QString clickedSceneNodeName = "";
                if (clickedSceneNode) {
                    clickedSceneNodeName = QString(clickedSceneNode->getName().c_str());

					//Log::debug(QString("Object %1 clicked.").arg(clickedSceneNodeName), "ViewportWidget::mousePressEvent");
					clickedSceneNodeName = clickedSceneNodeName.remove("SceneNode", Qt::CaseInsensitive);
					clickedSceneNodeName.remove("copy", Qt::CaseInsensitive);
					clickedSceneNodeName = clickedSceneNodeName.section('_', -1, -1);

					//Log::debug(QString("Object %1 clicked and cleaned.").arg(clickedNodeName), "ViewportWidget::mousePressEvent");
					// notify connected objects that an object in the viewport has been clicked
					// update the viewport to highlight selected objects
				}
				emit objectSelected(clickedSceneNodeName, event->modifiers() & Qt::ControlModifier);
                render();
            }
        }

    QWidget::mousePressEvent(event);
}


//!
//! Handles mouse move events for the widget.
//!
//! \param event The description of the mouse event.
//!
void ViewportWidget::mouseMoveEvent ( QMouseEvent *event )
{
    if (qApp->focusWidget() != this) {
        setViewMode(false);
        setFocus(Qt::MouseFocusReason);
    }

    const float dx = float(event->x() - m_lastPosition.x()) / height();
    const float dy = float(event->y() - m_lastPosition.y()) / height();
    m_lastPosition = event->pos();

    if (m_viewMode && event->buttons() & Qt::LeftButton) {
        // rotate view
        m_viewingParameters->rotate(180.0f * dx, 180.0f * dy);
        updateView();
        render();
    } else if (event->buttons() & Qt::MidButton) {
        // translate view
        m_viewingParameters->translate(dx, dy);
        updateView();
        render();
    } else if (m_viewMode && event->buttons() & Qt::RightButton) {
        // dolly view
        if (qAbs(dx) > qAbs(dy))
            m_viewingParameters->dolly(-dx);
        else
            m_viewingParameters->dolly(-dy);
        updateView();
        render();
    }

    QWidget::mouseMoveEvent(event);
}


//!
//! Handles mouse release events for the widget.
//!
//! \param event The description of the mouse event.
//!
void ViewportWidget::mouseReleaseEvent ( QMouseEvent *event )
{
    if (m_viewMode)
        setCursor(Qt::OpenHandCursor);
    else
        setCursor(Qt::ArrowCursor);

    QWidget::mouseReleaseEvent(event);
}


//!
//! Handles mouse wheel events for the widget.
//!
//! \param event The description of the mouse wheel event.
//!
void ViewportWidget::wheelEvent ( QWheelEvent *event )
{
    m_viewingParameters->dolly(event->delta(), true);
    updateView();
    render();
}

///
/// Private Slots
///
void ViewportWidget::updateTimer()
{
	/*if (m_leftTopHudTextArea) {
		m_leftTopHudTextArea->setCaption((m_imageInfo + QString::number((int) m_renderWindow->getAverageFPS())).toStdString());
		render();
	}*/
}


///
/// Private Functions
///


//!
//! Creates a unique name from the given object name by adding the viewport
//! widget's index.
//!
Ogre::String ViewportWidget::createUniqueName ( const QString &objectName )
{
    return QString("viewport%1_%2").arg(m_viewportNbr).arg(objectName).toStdString();
}


//!
//! Creates the default cg Shaders used for the default materials
//!
bool ViewportWidget::createdefaultShaders ()
{
	static const Ogre::String vertexShaderString =
		"void mainVP( in float4 iPosition : POSITION,\n"
						"out float4 oPosition : POSITION,\n"
						"uniform float4x4 worldViewProjMatrix )\n"
		"{\n"
			"oPosition = mul(worldViewProjMatrix, iPosition);\n"
		"}\n";

	static const Ogre::String vertexShaderLitString =
		"void mainVP( in float4 iPosition : POSITION,\n"
						"in float3 iNormal : NORMAL,\n"
						"out float4 oPosition : POSITION,\n"
						"out float3 oWPosition : TEXCOORD0,\n"
						"out float3 oVVec : TEXCOORD1,\n"
						"out float3 oNormal : NORMAL,\n"
						"uniform float4 iEPosition,\n"
						"uniform float4x4 worldViewProjMatrix )\n"
		"{\n"
			"oNormal = iNormal;\n"
			"oWPosition = iPosition.xyz;\n"
			"oVVec = iEPosition.xyz - iPosition.xyz;\n"
			"oPosition = mul(worldViewProjMatrix, iPosition);\n"
		"}\n";

	static const Ogre::String vertexShaderUVString =
		"void mainVP( in float4 iPosition : POSITION,\n"
						"in float2 iTexCoord : TEXCOORD0,\n"
						"out float4 oPosition : POSITION,\n"
						"out float2 oTexCoord : TEXCOORD0,\n"
						"uniform float4x4 worldViewProjMatrix )\n"
		"{\n"
			"oPosition = mul(worldViewProjMatrix, iPosition);\n"
			"oTexCoord = iTexCoord;\n"
		"}\n";
	
	static const Ogre::String vertexShaderFontString =
		"void mainVP( in float4 iPosition : POSITION,\n"
						"in float4 iColor : COLOR,\n"
						"in float2 iTexCoord : TEXCOORD0,\n"
						"out float4 oPosition : POSITION,\n"
						"out float4 oColor : COLOR,\n"
						"out float2 oTexCoord : TEXCOORD0 )\n"
		"{\n"
			"oPosition = iPosition;\n"
			"oColor = iColor;\n"
			"oTexCoord = iTexCoord;\n"
		"}\n";

	static const Ogre::String fragmentShaderString =
		"void mainFP( out float4 oColor : COLOR,\n"
						"uniform float4 color )\n"
		"{\n"
			"oColor = color;\n"
		"}\n";

	static const Ogre::String fragmentShaderLitString =
		"void mainFP( in float4 iPosition : POSITION,\n"
						"in  float3 iWPosition : TEXCOORD0,\n"
						"in  float3 iVVec : TEXCOORD1,\n"
						"in  float3 iNormal : NORMAL,\n"
						"out float4 oColor : COLOR,\n"
						"uniform float nbrLights,\n"
						"uniform float4 iLPositions[32],\n"
						"uniform float3 iLDiffColors[32],\n"
						"uniform float3 iLSpecColors[32],\n"
						"uniform float3 iLDirections[32],\n"
						"uniform float4 iLAttenuations[32],\n"
						"uniform float iLPowers[32] )\n"
		"{\n"
			"float3 wPosition = normalize(iWPosition);\n"
			"float3 vVec = normalize(iVVec);\n"
			"float3 normal = normalize(iNormal);\n"
			"float d, attn;\n"
			"float3 light = float3(0.f,0.f,0.f);\n"
			"float3 hVec, lVec;\n"
			"for (int i=0; i<nbrLights; i++) {\n"
				"if ( (iLSpecColors[i].x != 0.f) || (iLSpecColors[i].y != 0.f ) || (iLSpecColors[i].z != 0.f ))\n"
					"lVec = iLPositions[i].xyz - wPosition;\n"
				"else\n"
					"lVec = iLDirections[i];\n"
				"d = length(lVec);\n"
				"lVec = lVec/d;\n"
				"hVec = normalize(lVec + vVec);\n"
				"attn = ( 1.f / (( iLAttenuations[i].y ) + ( iLAttenuations[i].z * d ) + ( iLAttenuations[i].w * d * d )) );\n"
				"light +=\n"
					"( iLDiffColors[i] * max(dot(normal, lVec), 0.f) + \n"
					"  iLSpecColors[i] * pow(max(dot(normal, hVec), 0.f), 10.f) ) * \n"
					"iLPowers[i] * attn;\n"
			"}\n"
			"oColor = float4(light, 1.f);\n"
		"}\n";

	static const Ogre::String fragmentShaderTU0String =
		"void mainFP( in float4 iPosition : POSITION,\n"
						"in  float2 iTexCoord : TEXCOORD0,\n"
						"out float4 oColor : COLOR,\n"
						"uniform sampler2D tu0 : TEXUNIT0 )\n"
		"{\n"
			"oColor = tex2D(tu0, iTexCoord);\n"
		"}\n";

	static const Ogre::String fragmentShaderFontString =
		"void mainFP( in float4 iPosition : POSITION,\n"
						"in  float4 iColor : COLOR,\n"
						"in  float2 iTexCoord : TEXCOORD0,\n"
						"out float4 oColor : COLOR,\n"
						"uniform sampler2D tu0 : TEXUNIT0 )\n"
		"{\n"
			"oColor = tex2D(tu0, iTexCoord)*iColor;\n"
		"}\n";

	if (s_defaultVertexShader.isNull()) {
		s_defaultVertexShader = compileShader(vertexShaderString, Ogre::GPT_VERTEX_PROGRAM, "0");
		if (s_defaultVertexShader.isNull())
			return false;
	}
	if (s_defaultLitVertexShader.isNull()) {
		s_defaultLitVertexShader = compileShader(vertexShaderLitString, Ogre::GPT_VERTEX_PROGRAM, "1");
		if (s_defaultVertexShader.isNull())
			return false;
	}
	if (s_defaultUVVertexShader.isNull()) {
		s_defaultUVVertexShader = compileShader(vertexShaderUVString, Ogre::GPT_VERTEX_PROGRAM, "2");
		if (s_defaultUVVertexShader.isNull())
			return false;
	}
	if (s_defaultFontVertexShader.isNull()) {
		s_defaultFontVertexShader = compileShader(vertexShaderFontString, Ogre::GPT_VERTEX_PROGRAM, "3");
		if (s_defaultFontVertexShader.isNull())
			return false;
	}
	if (s_defaultFragmentShader.isNull()) {
		s_defaultFragmentShader = compileShader(fragmentShaderString, Ogre::GPT_FRAGMENT_PROGRAM, "0");
		if (s_defaultFragmentShader.isNull())
			return false;
	}
	if (s_defaultLitFragmentShader.isNull()) {
		s_defaultLitFragmentShader = compileShader(fragmentShaderLitString, Ogre::GPT_FRAGMENT_PROGRAM, "1");
		if (s_defaultFragmentShader.isNull())
			return false;
	}
	if (s_defaultTU0FragmentShader.isNull()) {
		s_defaultTU0FragmentShader = compileShader(fragmentShaderTU0String, Ogre::GPT_FRAGMENT_PROGRAM, "2");
		if (s_defaultTU0FragmentShader.isNull())
			return false;
	}
	if (s_defaultFontFragmentShader.isNull()) {
		s_defaultFontFragmentShader = compileShader(fragmentShaderFontString, Ogre::GPT_FRAGMENT_PROGRAM, "3");
		if (s_defaultFontFragmentShader.isNull())
			return false;
	}

	return true;
}


//!
//! Creates the default OpenGL3+ Shaders used for the default materials
//!
bool ViewportWidget::createDefaultShadersGL ()
{
	static const Ogre::String vertexShaderString =
		"#version 400 core \n"
		"uniform mat4 worldViewProjMatrix;\n"
		"in vec4 vertex;\n"
		"void main()\n"
		"{\n"
			"gl_Position = worldViewProjMatrix * vertex;\n"
		"}\n";
	
	static const Ogre::String vertexShaderLitString =
		"#version 400 core \n"
		"in vec4 vertex;\n"
		"in vec3 normal;\n"
		"out vec3 oVVec;\n"
		"out vec3 oNormal;\n"
		"out vec3 oWPosition;\n"
		"uniform vec4 iEPosition;\n"
		"uniform mat4 worldViewProjMatrix;\n"
		"void main()\n"
		"{\n"
			"oNormal = normal;\n"
			"oWPosition = vertex.xyz;\n"
			"oVVec = iEPosition.xyz - vertex.xyz;\n"
			"gl_Position = worldViewProjMatrix * vertex;\n"
		"}\n";

	static const Ogre::String vertexShaderUVString =
		"#version 400 core \n"
		"uniform mat4 worldViewProjMatrix;\n"
		"in vec4 vertex;\n"
		"in vec4 uv0;\n"
		"out vec2 oTexCoord;\n"
		"void main()\n"
		"{\n"
			"gl_Position = worldViewProjMatrix * vertex;\n"
			"oTexCoord = uv0.xy;\n"
		"}\n";

	static const Ogre::String vertexShaderFontString =
		"#version 400 core \n"
		"in vec4 vertex;\n"
		"in vec4 colour;\n"
		"in vec4 uv0;\n"
		"out vec4 vs_Color;\n"
		"out vec2 vs_TexCoord;\n"
		"void main()\n"
		"{\n"
			"gl_Position = vertex;\n"
			"vs_Color = colour;\n"
			"vs_TexCoord = uv0.xy;\n"
		"}\n";

	static const Ogre::String fragmentShaderString =
		"#version 400 core \n"
		"uniform vec4 color;\n"
		"out vec4 oColor;\n" 
		"void main()\n"
		"{\n"
			"oColor = color;\n"
		"}\n";

	static const Ogre::String fragmentShaderLitString =
		"#version 400 core\n"
		"in vec3 oWPosition;\n"
		"in vec3 oVVec;\n"
		"in vec3 oNormal;\n"
		"out vec4 color;\n"
		"uniform float nbrLights;\n"
		"uniform vec4 iLPositions[32];\n"
		"uniform vec3 iLDiffColors[32];\n"
		"uniform vec3 iLSpecColors[32];\n"
		"uniform vec3 iLDirections[32];\n"
		"uniform vec4 iLAttenuations[32];\n"
		"uniform float iLPowers[32];\n"
		"void main()\n"
		"{\n"
			"vec3 iWPosition = normalize(oWPosition);\n"
			"vec3 iNormal = normalize(oNormal);\n"
			"vec3 iVVec = normalize(oVVec);\n"
			"float d, attn;\n"
			"vec3 light = vec3(0.f,0.f,0.f);\n"
			"vec3 hVec, lVec;\n"
			"for (int i=0; i<nbrLights; i++) {\n"
			"if ( iLPositions[i].w == 1.f )\n"
					"lVec = iLPositions[i].xyz - iWPosition;\n"
				"else\n"
					"lVec = iLDirections[i];\n"
				"d = length(lVec);\n"
				"lVec = lVec/d;\n"
				"hVec = normalize(lVec + iVVec);\n"
				"attn = ( 1.f / (( iLAttenuations[i].y ) + ( iLAttenuations[i].z * d ) + ( iLAttenuations[i].w * d * d )) );\n"
				"light +=\n"
					"( iLDiffColors[i] * max(dot(iNormal, lVec), 0.f) + \n"
					"  iLSpecColors[i] * pow(max(dot(iNormal, hVec), 0.f), 10.f) ) * \n"
					"iLPowers[i] * attn;\n"
			"}\n"
			"color = vec4(light, 1.f);\n"
		"}\n";

	static const Ogre::String fragmentShaderTU0String =
		"#version 400 core \n"
		"uniform sampler2D tu0;\n"
		"in vec2 oTexCoord;\n"
		"out vec4 oColor;\n"
		"void main()\n"
		"{\n"
			"oColor = texture(tu0, oTexCoord);\n"
		"}\n";

	static const Ogre::String fragmentShaderFontString =
		"#version 400 core \n"
		"uniform sampler2D tu0;\n"
		"in vec4 vs_Color;\n"
		"in vec2 vs_TexCoord;\n"
		"out vec4 oColor;\n"
		"void main()\n"
		"{\n"
			"oColor = texture(tu0, vs_TexCoord)*vs_Color;\n"
		"}\n";

	if (s_defaultVertexShader.isNull()) {
		// Set Vertex Shader
		s_defaultVertexShader = compileShaderGL(vertexShaderString, Ogre::GPT_VERTEX_PROGRAM, "0");
		if (s_defaultVertexShader.isNull())
			return false;
	}
	if (s_defaultLitVertexShader.isNull()) {
		s_defaultLitVertexShader = compileShaderGL(vertexShaderLitString, Ogre::GPT_VERTEX_PROGRAM, "1");
		if (s_defaultLitVertexShader.isNull())
			return false;
	}
	if (s_defaultUVVertexShader.isNull()) {
		s_defaultUVVertexShader = compileShaderGL(vertexShaderUVString, Ogre::GPT_VERTEX_PROGRAM, "2");
		if (s_defaultUVVertexShader.isNull())
			return false;
	}
	if (s_defaultFontVertexShader.isNull()) {
		s_defaultFontVertexShader = compileShaderGL(vertexShaderFontString, Ogre::GPT_VERTEX_PROGRAM, "3");
		if (s_defaultFontVertexShader.isNull())
			return false;
	}
	if (s_defaultFragmentShader.isNull()) {
		// Set Fragment Shader
		s_defaultFragmentShader = compileShaderGL(fragmentShaderString, Ogre::GPT_FRAGMENT_PROGRAM, "0");
		if (s_defaultFragmentShader.isNull())
			return false;
	}
	if (s_defaultLitFragmentShader.isNull()) {
		// Set Fragment Shader
		s_defaultLitFragmentShader = compileShaderGL(fragmentShaderLitString, Ogre::GPT_FRAGMENT_PROGRAM, "1");
		if (s_defaultLitFragmentShader.isNull())
			return false;
	}
	if (s_defaultTU0FragmentShader.isNull()) {
		s_defaultTU0FragmentShader = compileShaderGL(fragmentShaderTU0String, Ogre::GPT_FRAGMENT_PROGRAM, "2");
		if (s_defaultTU0FragmentShader.isNull())
			return false;
	}
	if (s_defaultFontFragmentShader.isNull()) {
		s_defaultFontFragmentShader = compileShaderGL(fragmentShaderFontString, Ogre::GPT_FRAGMENT_PROGRAM, "3");
		if (s_defaultFontFragmentShader.isNull())
			return false;
	}

	return true;
}


//!
//! Compiles the given CG Shader Code and returns a Pointer to the loaded Ogre resource.
//! \param code The cg schader code.
//! \param profileFlags The shader profile flags.
//! \param type The type of the shader.
//! \return The pointer to the loaded shader.
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::compileShader ( const Ogre::String &code, Ogre::GpuProgramType type, const Ogre::String &nameSuffix )
{
	Ogre::String shaderName, shaderPostfix, profileFlag;
	Ogre::HighLevelGpuProgramPtr shaderPtr;
	
	const QString renderSystemName = OgreManager::getRenderSystemName();
	
	if (renderSystemName != "UNKNOWN")
	{
		if (type == Ogre::GPT_VERTEX_PROGRAM) {
			shaderName = createUniqueName("VertexProgram");
			shaderPostfix = "VP";
			if (renderSystemName.contains("11")) 		// DX11
				profileFlag = "vs_4_0";
			else if (renderSystemName.contains("9"))	// DX9
				profileFlag = "vs_1_1";
			else 										// GL
				profileFlag = "arbvp1";
		}
		else if (type == Ogre::GPT_FRAGMENT_PROGRAM) {
			shaderName = createUniqueName("FragmentProgram");
			shaderPostfix = "FP";
			if (renderSystemName.contains("11")) 		// DX11
				profileFlag = "ps_4_0";
			else if (renderSystemName.contains("9"))	// DX9
				profileFlag = "ps_2_0";
			else 										// GL
				profileFlag = "arbfp1";
		}

		// Set Fragment Shader
		shaderPtr = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram("FrapperDefault/" + shaderName + nameSuffix, "ViewportMaterials", "cg", type);
		shaderPtr->setSource(code);
		shaderPtr->setParameter("profiles", profileFlag);
		shaderPtr->setParameter("entry_point", "main" + shaderPostfix);
	
		if (shaderPtr->isSupported())
			shaderPtr->load();
		else
			shaderPtr.setNull();
	}

	return shaderPtr;
}


//!
//! Compiles the given OpenGL3+ Shader Code and returns a Pointer to the loaded Ogre resource.
//! \param code The OpenGL shader code.
//! \param profileFlags The shader profile flags.
//! \param type The type of the shader.
//! \return The pointer to the loaded shader.
//!
Ogre::HighLevelGpuProgramPtr ViewportWidget::compileShaderGL ( const Ogre::String &code, Ogre::GpuProgramType type, const Ogre::String &nameSuffix )
{
	Ogre::String shaderName;
	Ogre::HighLevelGpuProgramPtr shaderPtr;
	
	const QString renderSystemName = OgreManager::getRenderSystemName();
	
	if (renderSystemName != "UNKNOWN")
	{
		if (type == Ogre::GPT_VERTEX_PROGRAM) {
			shaderName = createUniqueName("VertexProgram");
		}
		else if (type == Ogre::GPT_FRAGMENT_PROGRAM) {
			shaderName = createUniqueName("FragmentProgram");
		}

		// Set Shader
		shaderPtr = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram("FrapperDefault/" + shaderName + nameSuffix, "ViewportMaterials", "glsl", type);
		shaderPtr->setSource(code);
	
		if (shaderPtr->isSupported())
			shaderPtr->load();
		else
			shaderPtr.setNull();
	}

	return shaderPtr;
}


//!
//! Creates the image material used by image overlay
//!
void ViewportWidget::createImageMaterial()
{
	// create init texture to prevent Ogre to crash if render system is DX11
	Ogre::TextureManager &textureManager = Ogre::TextureManager::getSingleton();
	static const Ogre::String defaultTexName = "Viewport/InitTex";
	if (!textureManager.resourceExists(defaultTexName))
		textureManager.createManual(defaultTexName, 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				Ogre::TEX_TYPE_2D, 
				1, 1, 0, 
				Ogre::PF_A8R8G8B8);
    
	// create material for image overlay
	m_imageMaterial = Ogre::MaterialManager::getSingleton().create(createUniqueName("imageMaterial"), "ViewportMaterials");
	Ogre::Pass *pass0 = m_imageMaterial->getTechnique(0)->getPass(0);
	pass0->setCullingMode(Ogre::CULL_NONE);
	pass0->setLightingEnabled(false);
	pass0->setDepthCheckEnabled(false);
	pass0->setDepthWriteEnabled(false);
	pass0->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	pass0->setVertexProgram(s_defaultUVVertexShader->getName());
	pass0->getVertexProgramParameters()->setNamedAutoConstant("worldViewProjMatrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
	pass0->setFragmentProgram(s_defaultTU0FragmentShader->getName());
	Ogre::TextureUnitState *textureUnitState = pass0->createTextureUnitState();
	textureUnitState->setTextureName(defaultTexName);
	textureUnitState->setNumMipmaps(0);
	textureUnitState->setTextureAnisotropy(1);
	textureUnitState->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
}

//!
//! Patches the Ogre Font Material to run without the fixed function pipeline.
//!
void ViewportWidget::createTextMaterial()
{
	Ogre::FontPtr fontPtr = Ogre::FontManager::getSingleton().getByName( "BlueHighway" );
	if (!fontPtr.isNull()) {
		fontPtr->load();
		Ogre::Pass *fontPass = fontPtr->getMaterial()->getTechnique(0)->getPass(0);
		if (fontPass) {
			fontPass->setVertexProgram(s_defaultFontVertexShader->getName());
			fontPass->setFragmentProgram(s_defaultFontFragmentShader->getName());
		}
	}
}

//!
//! Creates the OGRE render window.
//!
//! \return True if creating the OGRE render window succeeded, otherwise False.
//!
bool ViewportWidget::createRenderWindow ()
{
    Ogre::Root *ogreRoot = OgreManager::getRoot();
    if (!ogreRoot)
        return false;

	resize(parentWidget()->parentWidget()->size());  // SEIM: Hack to prevent dx11 resize bug
    
	// create a name/value list with miscellaneous parameters for the render window according to current OS
    Ogre::NameValuePairList miscParameters;
    if (!m_fullscreen) {
#if defined(Q_WS_X11)
        QWidget *q_parent = dynamic_cast<QWidget *>(parent());
        QX11Info xInfo = x11Info();
        miscParameters["parentWindowHandle"] = Ogre::StringConverter::toString((unsigned long) xInfo.display())
                                       + ":" + Ogre::StringConverter::toString((unsigned int) xInfo.screen())
                                       + ":" + Ogre::StringConverter::toString((unsigned long) q_parent->winId());
#else
        miscParameters["externalWindowHandle"] = Ogre::StringConverter::toString((size_t) (HWND) winId());
        miscParameters["FSAA"] = "4";
        miscParameters["vsync"] = "true";
#endif
    }

    // create the render window
    int windowWidth = width();
    int windowHeight = height();
    if (m_fullscreen) {
        windowWidth = m_width;
        windowHeight = m_height;
    }
    try {
        if (ogreRoot) {
			const bool initialised = ogreRoot->isInitialised();
			if (!initialised) {
				ogreRoot->initialise(false);
			}
            m_renderWindow = ogreRoot->createRenderWindow(createUniqueName("renderWindow"), windowWidth, windowHeight, false, &miscParameters);
        }
    }
    catch (Ogre::RenderingAPIException e) {
        m_renderWindow = 0;
        Log::error(QString("The render window could not be created: %1").arg(e.getDescription().c_str()), "ViewportWidget::createRenderWindow");
    }

//#if defined(Q_WS_X11)
//    if (m_renderWindow) {
//        WId windowId;
//        m_renderWindow->getCustomAttribute("GLXWINDOW", &windowId);
//
//        // take over the OGRE-created window
//        QWidget::create(windowId);
//
//        m_renderWindow->reposition(x(),y());
//    }
//#endif

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(2);

    return m_renderWindow != 0;
}


//!
//! Initializes the resources that will be available to OGRE.
//!
void ViewportWidget::initializeResources ()
{
	// create resource group
#if (OGRE_VERSION >= 0x010700)
    Ogre::ResourceGroupManager *resourceGroupManager = Ogre::ResourceGroupManager::getSingletonPtr();
    if (!resourceGroupManager->resourceGroupExists("ViewportMaterials"))
        resourceGroupManager->createResourceGroup("ViewportMaterials");
#endif 
	
	const QString renderSystemName = OgreManager::getRenderSystemName();
	
	// compiling the shaders for the viewport materials
	// OpenGL3+ renderer
	if(renderSystemName.contains("3+")){
		if(!createDefaultShadersGL()){
			Log::error("Error compiling viewport default shaders(OpenGL3+).", "ViewportWidget::initializeResources");
			return;
		}
	}
	// Cg renderer
	else if (!createdefaultShaders()) {
		 Log::error("Error compiling viewport default shaders.", "ViewportWidget::initializeResources");
		 return;
	}

	createImageMaterial();

	// Hack to prevent that frapper uses Ogres default -
	// fixed function "BaseWite" material if render system is DX11 or OpenGL3+
	if (renderSystemName.contains("11") 
		|| renderSystemName.contains("3+") 
		) {
		Ogre::MaterialManager::getSingleton().remove("BaseWhite");
		//createColorMaterial("Internal", "BaseWhite", Ogre::ColourValue(0.0f, 0.74902f, 0.0f), true);
		createBasicMaterial("Internal", "BaseWhite");
		Log::warning("Fixed Function not supported in DX11+, Ogre BaseWite material replaced.", "ViewportWidget::initializeResources");
	}

    if (s_redMaterial.isNull()) {
        // create color materials
        s_redMaterial = createColorMaterial("ViewportMaterials", "redMaterial", Ogre::ColourValue(1.0f, 0.0f, 0.0f), true);
        s_greenMaterial = createColorMaterial("ViewportMaterials", "greenMaterial", Ogre::ColourValue::Green, true);
        s_blueMaterial = createColorMaterial("ViewportMaterials", "blueMaterial", Ogre::ColourValue::Blue, true);
        s_grayMaterial = createColorMaterial("ViewportMaterials", "grayMaterial", Ogre::ColourValue(0.5f, 0.5f, 0.5f, 0.5f), true);
        s_blackMaterial = createColorMaterial("ViewportMaterials", "blackMaterial", Ogre::ColourValue::Black, true);
        s_defaultMaterial = createColorMaterial("ViewportMaterials", "defaultMaterial", Ogre::ColourValue(0.5f, 0.5f, 0.5f));
        s_nullMaterial = createColorMaterial("ViewportMaterials", "nullMaterial", Ogre::ColourValue(0.0f, 0.74902f, 0.0f), true);
    }

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	createTextMaterial();
}


//!
//! Creates basic viewport objects like the viewing camera, the head light,
//! coordinate axes and the grid.
//!
void ViewportWidget::createViewportObjects ()
{
    if (!m_sceneManager) {
        Log::error("The OGRE scene manager could not be obtained.", "ViewportWidget::createViewportObjects");
        return;
    }

    // create a faint ambient light
    m_sceneManager->setAmbientLight(Ogre::ColourValue(0.1f, 0.1f, 0.1f));

    // create the parent scene node for the scene node tree to which all viewport-specific objects will be attached
    m_viewportObjectsSceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode(createUniqueName("viewportObjects"));

    // create the main viewport camera
    m_camera = m_sceneManager->createCamera(createUniqueName("camera"));
    m_cameraSceneNode = m_viewportObjectsSceneNode->createChildSceneNode(createUniqueName("cameraSceneNode"));
    m_cameraSceneNode->attachObject(m_camera);

    // set additional camera info
    OgreContainer *cameraContainer = new OgreContainer(m_camera, width(), height());
    Ogre::MovableObject *movableObject = dynamic_cast<Ogre::MovableObject *>(m_camera);
    movableObject->setUserAny(Ogre::Any(cameraContainer));

    // create the orientation indicator viewport camera
    m_orientationIndicatorCamera = m_sceneManager->createCamera(createUniqueName("orientationIndicatorCamera"));
    m_orientationIndicatorCamera->setAspectRatio(1.0f);
    m_orientationIndicatorCamera->setNearClipDistance(0.01f);
    m_orientationIndicatorCamera->setFarClipDistance(100.0f);
    m_orientationIndicatorCamera->setFOVy(Ogre::Radian(Ogre::Degree(35)));
    m_orientationIndicatorCameraSceneNode = m_viewportObjectsSceneNode->createChildSceneNode(createUniqueName("orientationIndicatorCameraSceneNode"));
    m_orientationIndicatorCameraSceneNode->attachObject(m_orientationIndicatorCamera);

    // create the viewing camera's light
    m_headLight = m_sceneManager->createLight(createUniqueName("headLight"));
	m_headLight->setType(Ogre::Light::LT_POINT);
    m_headLight->setDiffuseColour(Ogre::ColourValue::White);
    //m_headLight->setSpecularColour(Ogre::ColourValue::White);
    m_cameraSceneNode->attachObject(m_headLight);
	m_headLight->setVisible(false);

    // set the default visibility flags for movable objects so that they show up in the main viewport
    Ogre::MovableObject::setDefaultVisibilityFlags(VF_MAIN_VIEWPORT);

    // create the main viewport covering the entire widget
    m_viewport = m_renderWindow->addViewport(m_camera);
    //Ogre::ColourValue backgroundColor(0.7f, 0.7f, 0.7f);
    m_viewport->setBackgroundColour(m_backgroundColor);
    //m_sceneManager->setFog(Ogre::FOG_LINEAR, backgroundColor, 0.0f, 1, 10);
    //m_sceneManager->setFog(Ogre::FOG_EXP, backgroundColor, 0.01);
    m_viewport->setVisibilityMask(VF_MAIN_VIEWPORT);

    // create the orientation indicator viewport covering only a corner of the widget
    m_orientationIndicatorViewport = m_renderWindow->addViewport(m_orientationIndicatorCamera, 1, 0.1f, 0.1f, 0.1f, 0.1f);
    m_orientationIndicatorViewport->setOverlaysEnabled(false);
    m_orientationIndicatorViewport->setVisibilityMask(VF_ORIENTATION_VIEWPORT);
    // disable clearing the color buffer so that the orientation indicator viewport acts as a transparent overlay on top of the main viewport
    m_orientationIndicatorViewport->setClearEveryFrame(false, Ogre::FBT_COLOUR);
    // enable clearing the depth buffer so that elements from the main viewport do not overlap elements in the orientation indicator viewport
    m_orientationIndicatorViewport->setClearEveryFrame(true, Ogre::FBT_DEPTH);

    // create the origin node
    m_originSceneNode = m_viewportObjectsSceneNode->createChildSceneNode(createUniqueName("originNode"));
    createAxes("origin", m_originSceneNode);

    // create the center point node
    m_centerPointSceneNode = m_viewportObjectsSceneNode->createChildSceneNode(createUniqueName("centerPointNode"));
    createAxes("centerPoint", m_centerPointSceneNode);

    // create the orientation indicator with a special visibility flag so that it only shows up in the orientation indicator viewport
    m_orientationIndicatorSceneNode = m_viewportObjectsSceneNode->createChildSceneNode(createUniqueName("orientationIndicatorNode"));
    createAxes("orientationIndicator", m_orientationIndicatorSceneNode, VF_ORIENTATION_VIEWPORT);

    // create the coordinate grid
    m_gridSceneNode = m_viewportObjectsSceneNode->createChildSceneNode(createUniqueName("gridNode"));
    createGrid(m_gridSceneNode);
}


//!
//! Creates overlay layers for visual components that will be rendered on
//! top of the main scene layer.
//!
void ViewportWidget::createOverlays ()
{
    Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	m_imageInfo = QString("Viewport\nFPS: ");

    // create the label text area overlay element
	const Ogre::String leftTopHudTextAreaName = QString(createUniqueName("leftTopHudTextArea_").c_str() + QString::number(m_stageIndex)).toStdString();
	if (overlayManager->hasOverlayElement(leftTopHudTextAreaName)) 
		m_leftTopHudTextArea = dynamic_cast<Ogre::TextAreaOverlayElement *>(overlayManager->getOverlayElement(leftTopHudTextAreaName));
	else {
		m_leftTopHudTextArea = dynamic_cast<Ogre::TextAreaOverlayElement *>(overlayManager->createOverlayElement("TextArea", leftTopHudTextAreaName));
		m_leftTopHudTextArea->setCaption((m_imageInfo + QString::number((int) m_renderWindow->getAverageFPS())).toStdString());
		m_leftTopHudTextArea->setMetricsMode(Ogre::GMM_RELATIVE);
		m_leftTopHudTextArea->setDimensions(1.0f, 1.0f);
		m_leftTopHudTextArea->setMetricsMode(Ogre::GMM_PIXELS);
		m_leftTopHudTextArea->setPosition(10, 10);
		m_leftTopHudTextArea->setFontName("BlueHighway");
		m_leftTopHudTextArea->setParameter("char_height", "16");
		m_leftTopHudTextArea->setColour(Ogre::ColourValue(.7f, .7f, .7f));
	}

	// create the label overlay element
	const Ogre::String leftTopHudPanelName = QString(createUniqueName("leftTopHudPanel_").c_str() + QString::number(m_stageIndex)).toStdString();
	if (overlayManager->hasOverlayElement(leftTopHudPanelName))
		m_leftTopHudPanel = dynamic_cast<Ogre::PanelOverlayElement *>(overlayManager->getOverlayElement(leftTopHudPanelName));
	else {
		m_leftTopHudPanel = dynamic_cast<Ogre::PanelOverlayElement *>(overlayManager->createOverlayElement("Panel", leftTopHudPanelName));
		m_leftTopHudPanel->setMetricsMode(Ogre::GMM_PIXELS);
		m_leftTopHudPanel->setDimensions(200, 100);
		m_leftTopHudPanel->setPosition(0, 0);
		//if (!OgreManager::getRenderSystemName().contains("13") &&
			m_leftTopHudPanel->addChild(m_leftTopHudTextArea);
		//}
	}

    // create the image overlay element
	const Ogre::String imagePanelName = QString(createUniqueName("imagePanel_").c_str() + QString::number(m_stageIndex)).toStdString();
	if (overlayManager->hasOverlayElement(imagePanelName))
		m_imagePanel = dynamic_cast<Ogre::PanelOverlayElement *>(overlayManager->getOverlayElement(imagePanelName));
	else {
		m_imagePanel = dynamic_cast<Ogre::PanelOverlayElement *>(overlayManager->createOverlayElement("Panel", imagePanelName));
		m_imagePanel->setMetricsMode(Ogre::GMM_PIXELS);
		m_imagePanel->setHorizontalAlignment(Ogre::GHA_CENTER);
		m_imagePanel->setVerticalAlignment(Ogre::GVA_CENTER);
	}
	m_imagePanel->setMaterialName(m_imageMaterial->getName());

    // create the label overlay
	const Ogre::String labelOverlayName = QString(createUniqueName("labelOverlay_").c_str() + QString::number(m_stageIndex)).toStdString();
	m_labelOverlay = overlayManager->getByName(labelOverlayName);
	if (!m_labelOverlay) {
		m_labelOverlay = overlayManager->create(labelOverlayName);
		m_labelOverlay->setZOrder(1);
		m_labelOverlay->add2D(m_leftTopHudPanel);
		m_labelOverlay->show();
	}

	// create the image overlay
	const Ogre::String imageOverlayName = QString(createUniqueName("imageOverlay_").c_str() + QString::number(m_stageIndex)).toStdString();
	m_imageOverlay = overlayManager->getByName(imageOverlayName);
	if (!m_imageOverlay) {
		m_imageOverlay = overlayManager->create(imageOverlayName);
		m_imageOverlay->setZOrder(0);
		m_imageOverlay->add2D(m_imagePanel);
	}
}


//!
//! Redraws the OGRE scene into the viewport widget.
//!
void ViewportWidget::render ()
{
    Ogre::Root *ogreRoot = OgreManager::getRoot();
    if (!ogreRoot || !m_renderWindow || !m_sceneManager)
        return;

    ogreRoot->_fireFrameStarted();

    // make the objects that are exclusive to this viewport visible    
    m_viewportObjectsSceneNode->setVisible(true); // also makes all child objects visible
    m_originSceneNode->setVisible(m_showOrigin);
    m_centerPointSceneNode->setVisible(m_showCenterPoint);
    m_orientationIndicatorSceneNode->setVisible(m_showOrientationIndicator);
    m_gridSceneNode->setVisible(m_showGrid);

    // hide all geometry only display overlay
    if (m_sceneManager->hasSceneNode("sceneRoot")) {    
        Ogre::SceneNode *sceneRoot = m_sceneManager->getSceneNode("sceneRoot");
        if (m_overlayOnly) {
            // make the objects that are exclusive to this viewport visible    
            sceneRoot->setVisible(false);
            // make the objects that are exclusive to this viewport visible    
            m_viewportObjectsSceneNode->setVisible(true); // also makes all child objects visible
            m_originSceneNode->setVisible(false);
            m_centerPointSceneNode->setVisible(false);
            m_orientationIndicatorSceneNode->setVisible(false);
            m_gridSceneNode->setVisible(false);
        }
        else {
            sceneRoot->setVisible(true);
        }
    }
    emit renderingTriggered();
    
	if (m_showOverlays)
		m_imageOverlay->show();
    
	m_leftTopHudTextArea->show();

    // update viewport
    m_renderWindow->update(false);
    // give client app opportunity to use queued GPU time
    ogreRoot->_fireFrameRenderingQueued();
    // block for final swap
    m_renderWindow->swapBuffers();
    
	m_imageOverlay->hide();
    m_leftTopHudTextArea->hide();

    // make the objects that are exclusive to this viewport invisible
    m_viewportObjectsSceneNode->setVisible(false); // also makes all child objects invisible

    ogreRoot->_fireFrameEnded();
}


//!
//! Calculates the transformation of the viewport's camera according to the
//! current viewing angles, the viewing radius, and the center of view.
//!
void ViewportWidget::updateView ()
{
    m_viewingParameters->update();

    // apply the viewing parameters to the viewport's camera
    m_viewingParameters->applyTo(m_cameraSceneNode);
    m_viewingParameters->applyTo(m_camera);

    // update the indicator viewport's camera
    m_viewingParameters->applyTo(m_orientationIndicatorCameraSceneNode, true);

    // update the center point node's transformations
    m_viewingParameters->applyTo(m_centerPointSceneNode, false, true);

    // notify connected objects that the viewing parameters have changed
    emit viewChanged(m_viewingParameters);
}


//!
//! Creates a self-illuminating material with the given name and color under
//! the given group name.
//!
//! \param group The name of the resource group to attach the material to.
//! \param name The unique name for the material.
//! \param color The color for the material.
//! \param wireframe Flag to control whether the material will display objects in wireframe mode.
//! \return The new color material.
//!
Ogre::MaterialPtr ViewportWidget::createColorMaterial ( const Ogre::String &group, const Ogre::String &name, const Ogre::ColourValue &color, bool wireframe /* = false */ )
{
	Ogre::MaterialPtr result = Ogre::MaterialManager::getSingleton().create(name, group).staticCast<Ogre::Material>();
	
	if (!result.isNull()) {
		Ogre::Pass *pass0 = result->getTechnique(0)->getPass(0);
		pass0->setCullingMode(Ogre::CULL_NONE);
		pass0->setLightingEnabled(false);

		if (wireframe)
			pass0->setPolygonMode(Ogre::PM_WIREFRAME);

		if (!s_defaultVertexShader.isNull()) {
			pass0->setVertexProgram(s_defaultVertexShader->getName());
			pass0->getVertexProgramParameters()->setNamedAutoConstant("worldViewProjMatrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
		}

		if (!s_defaultFragmentShader.isNull()) {
			pass0->setFragmentProgram(s_defaultFragmentShader->getName());
			pass0->getFragmentProgramParameters()->setNamedConstant("color", color);
		}
	}

    return result;
}


//!
//! Creates a simple nxl lit material with the given name and color under
//! the given group name.
//!
//! \param group The name of the resource group to attach the material to.
//! \param name The unique name for the material.
//! \param color The color for the material.
//! \return The new basic material.
//!
Ogre::MaterialPtr ViewportWidget::createBasicMaterial ( const Ogre::String &group, const Ogre::String &name)
{
	static const int maxNumberLights = 32;

	Ogre::MaterialPtr result = Ogre::MaterialManager::getSingleton().create(name, group).staticCast<Ogre::Material>();
	
	if (!result.isNull()) {
		Ogre::Pass *pass0 = result->getTechnique(0)->getPass(0);

		if (!s_defaultLitVertexShader.isNull()) {
			pass0->setVertexProgram(s_defaultLitVertexShader->getName());
			pass0->getVertexProgramParameters()->setNamedAutoConstant("iEPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
			pass0->getVertexProgramParameters()->setNamedAutoConstant("worldViewProjMatrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
		}

		if (!s_defaultLitFragmentShader.isNull()) {
			pass0->setFragmentProgram(s_defaultLitFragmentShader->getName());
			pass0->getFragmentProgramParameters()->setNamedAutoConstant("iLPositions", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY, maxNumberLights);
			pass0->getFragmentProgramParameters()->setNamedAutoConstant("iLDiffColors", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_ARRAY, maxNumberLights);
			pass0->getFragmentProgramParameters()->setNamedAutoConstant("iLSpecColors", Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR_ARRAY, maxNumberLights);
			pass0->getFragmentProgramParameters()->setNamedAutoConstant("iLPowers", Ogre::GpuProgramParameters::ACT_LIGHT_POWER_SCALE_ARRAY, maxNumberLights);
			pass0->getFragmentProgramParameters()->setNamedAutoConstant("iLDirections", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_OBJECT_SPACE_ARRAY, maxNumberLights);
			pass0->getFragmentProgramParameters()->setNamedAutoConstant("iLAttenuations", Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION_ARRAY, maxNumberLights);
			pass0->getFragmentProgramParameters()->setNamedAutoConstant("nbrLights", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
		}
	}

    return result;
}


//!
//! Creates three manual objects for X, Y and Z axes with the given name and
//! attaches them to the given parent scene node.
//! The axes will each have a length of 1.
//!
//! \param name The prefix for the axes objects to create.
//! \param parentNode The scene node to which to attach the axes objects.
//! \param visibilityFlags The visibility flags to apply to the axes objects.
//!
void ViewportWidget::createAxes ( const QString &name, Ogre::SceneNode *parentNode, Ogre::uint32 visibilityFlags /* = VF_MAIN_VIEWPORT */ )
{
    if (!m_sceneManager)
        return;

    Ogre::ManualObject *xAxis = m_sceneManager->createManualObject(createUniqueName(name + "_xAxis"));
    xAxis->begin("redMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    xAxis->position(0, 0, 0);
    xAxis->position(1, 0, 0);
    xAxis->end();
    xAxis->setVisibilityFlags(visibilityFlags);

    Ogre::ManualObject *yAxis = m_sceneManager->createManualObject(createUniqueName(name + "_yAxis"));
    yAxis->begin("greenMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    yAxis->position(0, 0, 0);
    yAxis->position(0, 1, 0);
    yAxis->end();
    yAxis->setVisibilityFlags(visibilityFlags);

    Ogre::ManualObject *zAxis = m_sceneManager->createManualObject(createUniqueName(name + "_zAxis"));
    zAxis->begin("blueMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    zAxis->position(0, 0, 0);
    zAxis->position(0, 0, 1);
    zAxis->end();
    zAxis->setVisibilityFlags(visibilityFlags);

    parentNode->attachObject(xAxis);
    parentNode->attachObject(yAxis);
    parentNode->attachObject(zAxis);
}


//!
//! Creates a coordinate grid and attaches it to the given parent scene node.
//!
//! \param parentNode The scene node to which to attach the grid object.
//!
void ViewportWidget::createGrid ( Ogre::SceneNode *parentNode )
{
    if (!m_sceneManager)
        return;

    int numRows = 32;
    float size = 16.0f;
    float halfSize = size / 2.0f;
    float sizeStep = size / numRows;
    float offset;

    Ogre::ManualObject *grid = m_sceneManager->createManualObject(createUniqueName("grid"));
    grid->begin("grayMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    for (int i = 0; i <= numRows; ++i)
        if (i != numRows / 2) {
            offset = i * sizeStep;
            // add a line parallel to the X axis
            grid->position(-halfSize, 0, -halfSize + offset);
            grid->position(+halfSize, 0, -halfSize + offset);
            // add a line parallel to the Z axis
            grid->position(-halfSize + offset, 0, -halfSize);
            grid->position(-halfSize + offset, 0, +halfSize);
        }
    grid->end();

    Ogre::ManualObject *gridCenter = m_sceneManager->createManualObject(createUniqueName("gridCenter"));
    gridCenter->begin("blackMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    // add a line through the X axis
    gridCenter->position(-halfSize, 0, 0);
    gridCenter->position(+halfSize, 0, 0);
    // add a line through the Z axis
    gridCenter->position(0, 0, -halfSize);
    gridCenter->position(0, 0, +halfSize);
    gridCenter->end();

    parentNode->attachObject(grid);
    parentNode->attachObject(gridCenter);
}


//!
//! Destroys the viwport widgeds overlay elements.
//!
void ViewportWidget::destroyOverlays ()
{
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();

		if (m_leftTopHudTextArea)
			overlayManager->destroyOverlayElement(m_leftTopHudTextArea);
		if (m_leftTopHudPanel)
			overlayManager->destroyOverlayElement(m_leftTopHudPanel);
		if (m_imagePanel)
			overlayManager->destroyOverlayElement(m_imagePanel);
		if (m_imageOverlay)
			overlayManager->destroy(m_imageOverlay);
		if (m_labelOverlay)
			overlayManager->destroy(m_labelOverlay);
}


//!
//! Returns the scene node that is located under the given mouse position in
//! the viewport.
//!
//! \param mousePosition The position of the mouse pointer over the viewport widget.
//! \return The clicked scene node or 0 if no scene node was clicked.
//!
Ogre::SceneNode * ViewportWidget::getClickedSceneNode ( const QPointF &mousePosition )
{
    if (!m_sceneManager || !m_renderWindow)
        return 0;

    Ogre::SceneNode *result = 0;
    Ogre::Ray mouseRay = m_camera->getCameraToViewportRay(mousePosition.x() / width(), mousePosition.y() / height());
    Ogre::RaySceneQuery *rayQuery = m_sceneManager->createRayQuery(mouseRay);
    rayQuery->setSortByDistance(true);
    Ogre::RaySceneQueryResult &rayQueryResult = rayQuery->execute();
    unsigned int i = 0;
    while (i < rayQueryResult.size() && !result) {
        Ogre::MovableObject *movableObject = rayQueryResult.at(i).movable;
        if (movableObject && movableObject->getVisible() && movableObject->getParentSceneNode() != m_gridSceneNode)
            result = movableObject->getParentSceneNode();
        else
            ++i;
    }
    m_sceneManager->destroyQuery(rayQuery);

    return result;
}

unsigned int ViewportWidget::getInstanceCount()
{
	return s_numberOfInstances;
}

} // end namespace Frapper
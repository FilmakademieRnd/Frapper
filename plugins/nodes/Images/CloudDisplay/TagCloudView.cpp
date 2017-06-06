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
//! \file "TagCloudView.cpp"
//! \brief Implementation file for TagCloudView class.
//!
//! \author     Diana Arellano
//! \version    1.0
//! \date       28.07.2009 (last updated)
//!

#include "TagCloudView.h"
#include "SegmentGraphicsItem.h"
#include "KeyGraphicsItem.h"
#include "NumberParameter.h"

#define scenescale 5 
#define MAX_VALUE 97384
#define MIN_VALUE 748
#define PI 3.14159265

//#define WIDTHSCENE 768
//#define HEIGHTSCENE 1280

namespace CloudDisplayNode {
using namespace Frapper;


///
/// Constructors and Destructors
///


//!
//! Constructor of the TagCloudView class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
TagCloudView::TagCloudView ( int width, int height, QWidget *parent /* = 0 */) :
    BaseGraphicsView(parent),
	m_scaleFactor(1.0)
{
    //QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    //graphicsScene->setSceneRect(-350, -250, 700, 500);
	//graphicsScene->setSceneRect(-(WIDTHSCENE/2), -(HEIGHTSCENE/2), WIDTHSCENE, HEIGHTSCENE);

	graphicsScene = new QGraphicsScene(this);

	heightimage = height;
	widthimage = width;

	graphicsScene->setSceneRect(-(width/2), -(height/2), width, height);
    setScene(graphicsScene);
	
	centerOn(0.0, 0.0);
	//centerOn(scene()->sceneRect().width()/2 ,scene()->sceneRect().height()/2);

	m_image = new QImage(QSize(width,height), QImage::Format_ARGB32);

    //setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorViewCenter);
    setResizeAnchor(AnchorViewCenter);

	biggestSize = 72;
	smallestSize = 30;
	dDeg = 30;
	useArea=false;
	dRadius=30.0;

	m_group = new QGraphicsItemGroup();

	m_offset = 0;
	m_finTimer = new QTimer();
	connect(m_finTimer, SIGNAL(timeout()), SLOT(updateTimer()));
}


//!
//! Destructor of the TagCloudView class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TagCloudView::~TagCloudView ()
{
}


///
/// Public Functions
///

//!
//! The overwritten the event handler for resize events.
//! Adds scene resizing and redrawing.
//!
//! \param event The description of the key event.
//!
void TagCloudView::resizeEvent ( QResizeEvent *event )
{

	scene()->setSceneRect(0, 0, width()*scenescale, height()*scenescale);
	centerOn(0,0);
    QGraphicsView::resizeEvent(event);
	qreal tagw = scene()->width();
}
//!
//! Resets the network graphics view's matrix.
//!
void TagCloudView::homeView ()
{
    setScale(1);
	QRectF boundingRect = scene()->itemsBoundingRect();
    centerOn(boundingRect.left() + boundingRect.width() , boundingRect.top() + boundingRect.height() );

}


void TagCloudView::createImage(int widthimg, int heightimg)
{	
	QSize size(widthimg, heightimg);
	m_image->scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation);

	m_image->fill(Qt::transparent);
	QPainter painter(m_image);	
	scene()->render(&painter);
	m_image->save("imagetest.png");
}

///
/// Public Slots
///

//!
//! Copies the images rendered in the QT widget into the Ogre renderer
//!
void TagCloudView::setImage(Node* selectedNode)
{
	ImageNode *imageNode = dynamic_cast<ImageNode *>(selectedNode);

	if(imageNode)
	{
		Ogre::TexturePtr ptr = imageNode->getImage();
		
		if(!ptr.isNull()) 
		{
			Ogre::HardwarePixelBufferSharedPtr tgtBuffer = ptr->getBuffer();
			tgtBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
			const Ogre::PixelBox &pixBox = tgtBuffer->getCurrentLock();
			
			//target Buffer
			uchar *tgtData = static_cast<uchar *>(pixBox.data);
			//source Buffer
			uchar *srcData = static_cast<uchar *>(m_image->bits());
			//copy data
			memcpy(tgtData, srcData, m_image->byteCount());
			tgtBuffer->unlock();		
		}
		else
			qWarning("Ogre::TexturePtr invalid");
	}
}

//!
//! Returns the image of the scene of this view
//!
QImage * TagCloudView::getImage ()
{
	return m_image;
}

void TagCloudView::cloudVisualization(QTableWidget* m_wordsTable, int rowsInTable, qreal m_fontSize, QString m_font, int width, int height, int posX, int posY, QColor color, Node* nodeSelected)	// Rendering and Visualization of the TagCloud
{				
	    int wid = 0;
		int hei = 0;
		RenderCloud(m_wordsTable, rowsInTable, m_fontSize, m_font, wid, hei, posX, posY, color);
		createImage(width, height);
		setImage(nodeSelected);	// Creation of the image	
}



QList<int> TagCloudView::createShuffledListIndexes(int rows)
{
	//QStringList *list = new QStringList();
	QList<int> list;

	for (int i=0; i<rows; i++)
	{
		list.append(i);
	}
	//shuffle
	// Create seed for the random
	// That is needed only once on application startup
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	for (int i=0; i<rows-1; i++)
	{
		// Get random value between i+1 and rows
		int low = i+1;
		int randomValue = qrand() % ((rows) - low) + low;
		int tmp = list.at(i);
		list[i] = list.at(randomValue);
		list[randomValue] = tmp;
	}

	return list;
}

//!
//!
void TagCloudView::translateInScene(int width, int height, Node* nodeSelected)
{		
	// Translates the whole group of text items
	if (m_group != NULL)
	{
		QTransform translationTransform(1, 0, 0, 1, qreal(width), qreal(height));
		//m_group->setTransform(translationTransform);
		if (qreal(width) == 0.0)
			m_group->setPos(m_group->x(), qreal(height));
		else if (qreal(height) == 0.0)
			m_group->setPos(qreal(width), m_group->y());
		else
			m_group->setPos(qreal(width),qreal(height));
		

		m_image->fill(Qt::transparent);
		QPainter painter(m_image);
		scene()->render(&painter);

		setImage(nodeSelected);
	}
}

//!
//!
void TagCloudView::translateItemsInScene(int rowsInTable, QTableWidget *wordsTable, float offset, Node* nodeSelected)
{
	qreal x122, x222, y122, y222;

	m_imageLeftLimit = m_group->sceneBoundingRect().x(); 
	m_imageUpperLimit = m_group->sceneBoundingRect().y();
	m_imageRightLimit = m_group->sceneBoundingRect().width();
	m_imageLowerLimit = m_group->sceneBoundingRect().height();

	for(int i=0; i<m_graphicsList.size(); i++) {

		QGraphicsTextItem *current = m_graphicsList.at(i);
		QRectF bprev = current->sceneBoundingRect();

		x122=bprev.x();
		x222=bprev.width();
		y122=bprev.y();
		y222=bprev.height();
		
		if (y122 < m_imageUpperLimit)
		{
			current->moveBy(0, m_imageLowerLimit);
		}
		else
		{
			current->moveBy(0, offset);
		}
	}

	//! paint the image
	m_image->fill(Qt::transparent);
	QPainter painter(m_image);
	scene()->render(&painter);
	setImage(nodeSelected);
}


//!
//!
void TagCloudView::RenderCloud(QTableWidget *wordsTable, int rowsInTable, qreal fontSize, QString fontName, int &finalW, int &finalH, int posX, int posY, QColor color){

	//We define the min and max frequencies of the words
	qreal high = MAX_VALUE;
	qreal low = MIN_VALUE;
	bool b_first = true;

	m_itemsIndexes.clear();
	m_itemsIndexes = createShuffledListIndexes(rowsInTable);
	
	// To use just the first 30 words of the table
	if (rowsInTable > 25)
		rowsInTable = 25;

	scene()->clear();

	//QList<QGraphicsTextItem *> graphicsList;
	m_graphicsList.clear();

	for(int i=0; i<rowsInTable; i++) {

		int idx = m_itemsIndexes.at(i);
		QTableWidgetItem *wordItem = new QTableWidgetItem;
		wordItem = wordsTable->item(idx,1);
		qreal freq = wordItem->text().toDouble();
		freq = freq * fontSize;
		wordItem->setText(QString::number(freq));

		high = MAX(high, freq);
		low = MIN(low, freq);
	}	

	QString word = wordsTable->item(0,0)->text();
	qreal freq = wordsTable->item(0,1)->text().toDouble();

	m_first = new QGraphicsTextItem;
	QFont wordItemFont = m_first->font();
	//wordItemFont.setFamily("Lucida Calligraphy");
	wordItemFont.setFamily(fontName);
	wordItemFont.setPointSize( (int)(freq) );

	m_first->setDefaultTextColor(color);
	m_first->setFont(wordItemFont);
	m_first->setPlainText(word);
	

	for(int i=0; i<rowsInTable; i++) {
		
		int idx = m_itemsIndexes.at(i);

		QString word = wordsTable->item(idx,0)->text();

		QTableWidgetItem *wordItem = new QTableWidgetItem;
		wordItem = wordsTable->item(idx,1);
		qreal freq = wordItem->text().toDouble();

		QGraphicsTextItem *graphicsItem = new QGraphicsTextItem;
		QFont wordItemFont = graphicsItem->font();
		//wordItemFont.setFamily("Lucida Calligraphy");
		wordItemFont.setFamily(fontName);
		wordItemFont.setPointSize( (int)(freq) );
		wordItemFont.setBold(true);
		graphicsItem->setDefaultTextColor(color);
		graphicsItem->setFont(wordItemFont);
		graphicsItem->setPlainText(word);
		
		//We translate the QGraphicsTextItem
		QRectF bounds = graphicsItem->sceneBoundingRect();
		qreal x1=0;
		qreal x2=0;
		qreal y1=0;
		qreal y2=0;
		//bounds.getCoords(&x1,&y1,&x2,&y2);

		qreal xx = -bounds.center().x();
		qreal yy = -bounds.center().y();
	
		graphicsItem->moveBy(xx, yy);
		//bounds = graphicsItem->sceneBoundingRect();
		//bounds.getCoords(&x1,&y1,&x2,&y2);
		m_graphicsList.append(graphicsItem);

		scene()->addItem(graphicsItem);		

	}

	
	distributeWordItems(rowsInTable, wordsTable);	
	

	qreal wscene = scene()->sceneRect().width();
	qreal hscene = scene()->sceneRect().height();
	QPointF pointf(wscene, hscene);
	QPoint pointt = pointf.toPoint();
	finalW = pointt.rx();
	finalH = pointt.ry();

	// Group all selected items together
	m_group = scene()->createItemGroup(scene()->items());
	QRectF allbound = m_group->sceneBoundingRect();
	qreal x1=0;
	qreal x2=0;
	qreal y1=0;
	qreal y2=0;
	allbound.getCoords(&x1,&y1,&x2,&y2);
	qreal wgroup = allbound.width();
	qreal hgroup = allbound.height();

	qreal newx = (x1 + (wgroup/2)) * (-1.0);
	qreal newy = (y1 + (hgroup/2)) * (-1.0);

	m_group->moveBy(newx, newy);	//centers the cloud ???

	m_group->moveBy(posX, posY);
	//m_group->setPos(posX, posY);

	m_imageLeftLimit = m_group->sceneBoundingRect().x(); 
	m_imageUpperLimit = m_group->sceneBoundingRect().y();
	m_imageRightLimit = m_group->sceneBoundingRect().width();
	m_imageLowerLimit = m_group->sceneBoundingRect().height();

	//scene()->addRect(group->sceneBoundingRect(), QPen(Qt::SolidLine), QBrush(Qt::transparent));
}

//!
//!
void TagCloudView::distributeWordItems(int rowsInTable, QTableWidget *wordsTable)
{
	//first point
	QPointF *center = new QPointF(0.0,0.0);

	for(int i=1; i<rowsInTable; i++) {

		QGraphicsTextItem *current = m_graphicsList.at(i);

		//calculate current center
		center->setX(0.0);
		center->setY(0.0);
		qreal totalWeight = 0.0;

		for (int prev=0; prev<i; ++prev){

			int idx = m_itemsIndexes.at(prev);

			QGraphicsTextItem *wPrev = m_graphicsList.at(prev);
			qreal freq = wordsTable->item(idx,1)->text().toDouble();
			QRectF bprev = wPrev->sceneBoundingRect();

			qreal x122=0;
			qreal x222=0;
			qreal y122=0;
			qreal y222=0;
			bprev.getCoords(&x122,&y122,&x222,&y222);

			qreal wPrevCenterX = bprev.center().x();
			qreal wPrevCenterY = bprev.center().y();

			qreal newX = center->x() + (wPrevCenterX * freq);
			qreal newY = center->y() + (wPrevCenterY * freq);
			center->setX(newX);
			center->setY(newY);

			totalWeight += freq;
		}

		center->setX(center->x()/totalWeight);
		center->setY(center->y()/totalWeight);

		bool done = false;
		qreal fw = m_first->sceneBoundingRect().width();
		qreal fh = m_first->sceneBoundingRect().height();
		//qreal radius = 0.5 * MIN(fw, fh);
		qreal radiusMa = 0.5 * fw;
		qreal radiusMi = 0.5 * fh;

		while (!done){

			//qWarning(" %d/%d radiusMa: %f, radiousMi: %f", i, rowsInTable, radiusMa, radiusMi);

			int startDeg = qrand() % 359;
			//int startDeg = 41;	//only works with global variables 'dRadius'=30 and 'dDeg'=30
			//loop over the spiral
			int prev_x = -1;
			int prev_y = -1;

			for (int deg=startDeg; deg<startDeg+360; deg+=dDeg) {

				//double rad=((double)deg/PI)*180.0;
				double rad=((double)deg*PI)/180.0;
				int cx=(int)(center->x() + (radiusMa * qCos(rad)) );
				int cy=(int)(center->y() + (radiusMi * qSin(rad)) );

				if(prev_x == cx && prev_y == cy) continue;
				prev_x=cx;
				prev_y=cy;

				QTransform candidate;
				candidate.translate(cx,cy);

				QRectF curect = current->sceneBoundingRect();
				qreal cux1 = 0;
				qreal cuy1 = 0;
				qreal cux2 = 0;
				qreal cuy2 = 0;
				curect.getCoords(&cux1, &cuy1, &cux2, &cuy2);

				QRectF *bound1;

				if (!useArea){
					qreal bx = current->sceneBoundingRect().x(); 
					qreal by = current->sceneBoundingRect().y();
					qreal ww = current->sceneBoundingRect().width();
					qreal hh = current->sceneBoundingRect().height();
					bound1 = new QRectF(bx + cx, by + cy, ww, hh);
				}

				//any collision ?
				int prev=0;
				for (prev=0; prev<i; ++prev){
					if (!useArea) {

						QRectF bound2 = m_graphicsList.at(prev)->sceneBoundingRect();

						if ( bound1->intersects(bound2) ) break;						
					}
				}
				//no collision: we are done
				if (prev==i) {
					current->setTransform(candidate);
					qreal bx = current->sceneBoundingRect().x(); 
					qreal by = current->sceneBoundingRect().y();
					qreal ww = current->sceneBoundingRect().width();
					qreal hh = current->sceneBoundingRect().height();
					qWarning("item %d: %f %f %f %f \n", i, bx, by, ww, hh);

					done = true;
					break;
				}
			}
			radiusMa += dRadius;
			radiusMi += (dRadius * 0.35);
		}
	}

	
}


void TagCloudView::updateTimer()
{
	m_offset = m_offset -5;
}

} // namespace CloudDisplayNode 

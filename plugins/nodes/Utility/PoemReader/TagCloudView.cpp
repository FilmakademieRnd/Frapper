/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2014 Filmakademie Baden-Wuerttemberg, Institute of Animation 

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
//! \author     Simon Spielmann <sspielma@filmakademie.de>
//! \author     Felix Bucella <felix.bucella@filmakademie.de>
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
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
#define WIDTHSCENE 768
#define HEIGHTSCENE 1280

namespace PoemReaderNode {
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
TagCloudView::TagCloudView ( QWidget *parent /* = 0 */) :
    BaseGraphicsView(parent),
	m_scaleFactor(1.0)
{
    QGraphicsScene *graphicsScene = new QGraphicsScene(this);
    //graphicsScene->setSceneRect(-350, -250, 700, 500);
	graphicsScene->setSceneRect(-(WIDTHSCENE/2), -(HEIGHTSCENE/2), WIDTHSCENE, HEIGHTSCENE);

	//graphicsScene->setSceneRect(0, 0, width()*3, height()*3);
	//graphicsScene->setSceneRect(-width()/2, height()/2, width(), height());

    setScene(graphicsScene);
	graphicsScene->setBackgroundBrush(QBrush(QColor(0,0,122,0)));
	

	//centerOn(scene()->sceneRect().width()/2 ,scene()->sceneRect().height()/2);
	centerOn(0.0, 0.0);

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
	//scene()->setSceneRect(-width()/2, height()/2, width()*scenescale, height()*scenescale);
	

	//centerOn(0,scene()->sceneRect().height()/2);
	centerOn(0,0);
    QGraphicsView::resizeEvent(event);
	qreal tagw = scene()->width();
}



//!
//! The overwritten event "fills" the QImage with the content of the Qt scene 
//!
//void TagCloudView::paintEvent(QPaintEvent *event)
//{
//	BaseGraphicsView::paintEvent(event);
//	
//	m_image = new QImage(QSize(400, 300), QImage::Format_ARGB32);
//	//m_image->fill(qRgba(0,255,0,0));	
//	m_image->fill(Qt::transparent);
//
//	QPainter painter(m_image);
//	scene()->render(&painter);
//
//	m_image->save("imagetest.png");
//}

void TagCloudView::createImage(){

	//m_image = new QImage(QSize(700,500), QImage::Format_ARGB32);
	m_image = new QImage(QSize(WIDTHSCENE,HEIGHTSCENE), QImage::Format_ARGB32);
	m_image->fill(Qt::transparent);

	QPainter painter(m_image);
	scene()->render(&painter);

	m_image->save("imagetest.png");
}

///
/// Public Slots
///

//!
//! Resets the network graphics view's matrix.
//!
void TagCloudView::homeView ()
{
    setScale(1);
	QRectF boundingRect = scene()->itemsBoundingRect();
    centerOn(boundingRect.left() + boundingRect.width() , boundingRect.top() + boundingRect.height() );

}

//!
//! Copies the images rendered in the QT widget into the Ogre renderer
//!
void TagCloudView::setImage(Node* selectedNode)
{
	ImageNode *imageNode = dynamic_cast<ImageNode *>(selectedNode);

	if(imageNode)
	{
		Ogre::TexturePtr ptr = imageNode->getImage();

		if(!ptr.isNull()) {
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

void TagCloudView::RenderCloud(QTableWidget *wordsTable, int rowsInTable){

	//We define the min and max frequencies of the words
	qreal high = MAX_VALUE;
	qreal low = MIN_VALUE;
	bool b_first = true;
	QGraphicsTextItem *first;

	
	// To use just the first 30 words of the table
	if (rowsInTable > 30)
		rowsInTable = 30;
		

	QList<QGraphicsTextItem *> graphicsList;

	for(int i=0; i<rowsInTable; i++) {

		QTableWidgetItem *frequencyItem = new QTableWidgetItem;
		frequencyItem = wordsTable->item(i,1);
		qreal freq = frequencyItem->text().toDouble();
		freq = freq * 4.0;
		frequencyItem->setText(QString::number(freq));
		//wordsTable->setItem(i,1,frequencyItem);

		high = MAX(high, freq);
		low = MIN(low, freq);
	}	

	QString word = wordsTable->item(0,0)->text();
	qreal freq = wordsTable->item(0,1)->text().toDouble();

	first = new QGraphicsTextItem;

	

	QFont wordItemFont = first->font();
	wordItemFont.setFamily("Lucida Calligraphy");
	//wordItemFont.setPointSize( (int)(((freq-low)/(high-low))*(this->biggestSize - this->smallestSize))+this->smallestSize );
	wordItemFont.setPointSize( (int)(freq) );

	first->setDefaultTextColor(QColor(255,23,15,255));
	first->setFont(wordItemFont);
	first->setPlainText(word);

	for(int i=0; i<rowsInTable; i++) {
		
		//QTableWidgetItem *wordItem = new QTableWidgetItem;
		//wordItem = wordsTable->item(i,0);
		//QString word = wordItem->text();
		QString word = wordsTable->item(i,0)->text();

		QTableWidgetItem *frequencyItem = new QTableWidgetItem;
		frequencyItem = wordsTable->item(i,1);
		qreal freq = frequencyItem->text().toDouble();
		//qreal freq =  wordsTable->item(i,1)->text().toDouble();

		QGraphicsTextItem *graphicsItem = new QGraphicsTextItem;

		QFont wordItemFont = graphicsItem->font();
		wordItemFont.setFamily("Lucida Calligraphy");
		//wordItemFont.setPointSize( (int)(((freq-low)/(high-low))*(this->biggestSize - this->smallestSize))+this->smallestSize );
		wordItemFont.setPointSize( (int)(freq) );
		wordItemFont.setBold(true);
		
		graphicsItem->setDefaultTextColor(QColor(255,255,255,255));
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
		graphicsList.append(graphicsItem);

		scene()->addItem(graphicsItem);
		
	}

	//first point
	QPointF *center = new QPointF(0.0,0.0);
	
	for(int i=1; i<rowsInTable; i++) {
		
		QGraphicsTextItem *current = graphicsList.at(i);

		//calculate current center
		center->setX(0.0);
		center->setY(0.0);
		qreal totalWeight = 0.0;

		for (int prev=0; prev<i; ++prev){
			
			QGraphicsTextItem *wPrev = graphicsList.at(prev);
			qreal freq = wordsTable->item(prev,1)->text().toDouble();
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
		qreal fw = first->sceneBoundingRect().width();
		qreal fh = first->sceneBoundingRect().height();
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

			for (int deg=startDeg; deg<startDeg+360; deg+=dDeg){
			
				
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

				if (useArea){
					
				}
				else {
					qreal bx = current->sceneBoundingRect().x(); 
					qreal by = current->sceneBoundingRect().y();
					qreal ww = current->sceneBoundingRect().width();
					qreal hh = current->sceneBoundingRect().height();
					bound1 = new QRectF(bx + cx, by + cy, ww, hh);
				}

				//any collision ?
				int prev=0;
				for (prev=0; prev<i; ++prev){
					if (useArea){
						
					}
					else {
						QRectF bound2 = graphicsList.at(prev)->sceneBoundingRect();
						
						if ( bound1->intersects(bound2) ){
							break;
						}
					}
				}
				//no collision: we are done
				if (prev==i) {
					current->setTransform(candidate);
					done = true;
					break;
				}
			}
			radiusMa += dRadius;
			radiusMi += (dRadius * 0.35);
		}
	}

	// Group all selected items together
	QGraphicsItemGroup *group = scene()->createItemGroup(scene()->items());
	QRectF allbound = group->sceneBoundingRect();
	qreal x1=0;
	qreal x2=0;
	qreal y1=0;
	qreal y2=0;
	allbound.getCoords(&x1,&y1,&x2,&y2);

	//qreal x = (-1.0)* (WIDTHSCENE/3);
	qreal x = -(allbound.width() / 2);
	qreal y = HEIGHTSCENE/4;
	group->moveBy(x-x1, y-y1);

	allbound = group->sceneBoundingRect();
	allbound.getCoords(&x1,&y1,&x2,&y2);
}

} // namespace PoemReaderNode 

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
//! \file "TimelineWidget.cpp"
//! \brief Implementation file for TimelineWidget class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       30.06.2009 (last updated)
//!

#include "TimelineWidget.h"
#include <QPainter>
#include <QScrollBar>
#include "Log.h"


namespace Frapper {

///
/// Public Static Data
///


//!
//! The default size of frame rectangles (should be odd numbers).
//!
const QSize TimelineWidget::DefaultFrameSize = QSize(5, 17); 


///
/// Constructors and Destructors
///


//!
//! Constructor of the TimelineWidget class.
//!
//! \param parent The parent widget the created instance will be a child of.
//! \param flags Extra widget options.
//!
TimelineWidget::TimelineWidget ( QWidget *parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) :
    QWidget(parent, flags),
    m_parentScrollArea(dynamic_cast<QScrollArea *>(parent)),
    m_startGrabber(new GrabberWidget(DefaultFrameSize, GrabberWidget::P_BeforeFrame, palette().color(QPalette::Button), this)),
    m_endGrabber(new GrabberWidget(DefaultFrameSize, GrabberWidget::P_AfterFrame, palette().color(QPalette::Button), this)),
    m_inGrabber(new GrabberWidget(DefaultFrameSize, GrabberWidget::P_BeforeFrame, palette().color(QPalette::Mid), this)),
    m_outGrabber(new GrabberWidget(DefaultFrameSize, GrabberWidget::P_AfterFrame, palette().color(QPalette::Mid), this)),
    m_currentGrabber(new GrabberWidget(DefaultFrameSize, GrabberWidget::P_OnFrame, QColor(0, 152, 206), this)),
    m_movedGrabber(0),
    m_currentFrameEdit(new QLineEdit(this)),
    m_currentFrameValidator(new QIntValidator(this)),
    m_autoScroll(true),
    m_frameWidth(DefaultFrameSize.width()),
    m_frameHeight(DefaultFrameSize.height()),
    m_tickStep(10)
{
    setMinimumHeight(m_frameHeight + 20);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // define colors to use for the current frame line edit
    QString buttonColorName (palette().color(QPalette::Button).name());
    QString midColorName (palette().color(QPalette::Button).name());

    // initialize the current frame line edit
    m_currentFrameEdit->setObjectName("m_currentFrameEdit");
    m_currentFrameEdit->setVisible(false);
    m_currentFrameEdit->setAlignment(Qt::AlignCenter);
    m_currentFrameEdit->resize(QSize(50, 16));
    m_currentFrameEdit->setValidator(m_currentFrameValidator);
    QFont font (m_currentFrameEdit->font());
    font.setBold(true);
    m_currentFrameEdit->setFont(font);
    m_currentFrameEdit->setAutoFillBackground(false);
    m_currentFrameEdit->setStyleSheet(QString("background-color: %1; border: 1px solid %2;").arg(buttonColorName, midColorName));

    if (m_parentScrollArea)
        // set up a signal/slot connection to always update the full timeline widget whenever the parent scroll area's
        // horizontal scroll bar value is changed
        connect(m_parentScrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(update()));

    QMetaObject::connectSlotsByName(this);
}


//!
//! Destructor of the TimelineWidget class.
//!
//! Defined virtual to guarantee that the destructor of a derived class
//! will be called if the instance of the derived class is saved in a
//! variable of its parent class type.
//!
TimelineWidget::~TimelineWidget ()
{
}


///
/// Public Functions
///


//!
//! Sets the index of the current frame in the scene's time.
//!
//! \param index The new index of the current frame in the scene's time.
//!
void TimelineWidget::setCurrentFrame ( int index )
{
    m_currentGrabber->setFrameIndex(index);

    if (m_parentScrollArea && m_autoScroll) {
        int x = (m_currentGrabber->getFrameIndex() - m_startGrabber->getFrameIndex()) * (m_frameWidth - 1) + GrabberWidget::Width - 1;
        //Log::debug(QString("Scrolling frame %1 (x = %2) into view.").arg(index).arg(x), "TimelineWidget::setCurrentFrame");
        m_parentScrollArea->ensureVisible(x, 0);
    }

    update();
}


//!
//! Sets the index of the start frame in the scene's time.
//!
//! \param index The new index of the start frame in the scene's time.
//!
void TimelineWidget::setStartFrame ( int index )
{
    m_startGrabber->setFrameIndex(index);

    // notify all grabbers of the new start frame index
    m_startGrabber->setStartFrameIndex(index);
    m_endGrabber->setStartFrameIndex(index);
    m_inGrabber->setStartFrameIndex(index);
    m_outGrabber->setStartFrameIndex(index);
    m_currentGrabber->setStartFrameIndex(index);

    updateWidth();
    update();
}


//!
//! Sets the index of the end frame in the scene's time.
//!
//! \param index The new index of the start frame in the scene's time.
//!
void TimelineWidget::setEndFrame ( int index )
{
    m_endGrabber->setFrameIndex(index);

    updateWidth();
    update();
}


//!
//! Sets the index of the in frame in the scene's time.
//!
//! \param index The new index of the in frame in the scene's time.
//!
void TimelineWidget::setInFrame ( int index )
{
    m_inGrabber->setFrameIndex(index);
    update();
}


//!
//! Sets the index of the out frame in the scene's time.
//!
//! \param index The new index of the out frame in the scene's time.
//!
void TimelineWidget::setOutFrame ( int index )
{
    m_outGrabber->setFrameIndex(index);
    update();
}


//!
//! Calculates the optimum frame width for fitting all frames from start to
//! end frame (or in to out frame) into the timeline widgets dimensions.
//!
//! \param inToOut Flag to control whether to calculate the optimum frame width for the range from in to out frame instead of the start to end frame.
//! \return The frame width at which all frames in the frame range fit into the timeline widgets dimensions.
//!
int TimelineWidget::calculateFrameWidth ( bool inToOut /* = false */ )
{
    GrabberWidget *firstGrabber = m_startGrabber;
    GrabberWidget *lastGrabber = m_endGrabber;
    if (inToOut) {
        firstGrabber = m_inGrabber;
        lastGrabber = m_outGrabber;
    }
    int numberOfFrames = lastGrabber->getFrameIndex() - firstGrabber->getFrameIndex() + 1;
    int w = m_parentScrollArea->width() - m_startGrabber->width() - m_endGrabber->width();
    int frameWidth = w / numberOfFrames;

    // check if the calculated frame width is an even number
    if (frameWidth % 2 == 0)
        // round the calculated frame width to an odd number
        if (frameWidth * numberOfFrames < w)
            ++frameWidth;
        else
            --frameWidth;

    return frameWidth;
}


///
/// Public Slots
///


//!
//! Enables or disables automatic scrolling of the widget's parent scroll
//! area to ensure the current frame is visible.
//!
//! \param autoScroll The new value for the automatic scrolling flag.
//!
void TimelineWidget::setAutoScroll ( bool autoScroll )
{
    m_autoScroll = autoScroll;
}


//!
//! Sets the width of frame rectangles to the given width.
//!
//! \param frameWidth The width to use for frame rectangles.
//!
void TimelineWidget::setFrameWidth ( int frameWidth )
{
    m_frameWidth = frameWidth;
    if (m_frameWidth > 17)
        m_tickStep = 5;
    else if (m_frameWidth > 3)
        m_tickStep = 10;
    else
        m_tickStep = 20;

    // notify all grabbers of the new frame width
    m_startGrabber->setFrameWidth(frameWidth);
    m_endGrabber->setFrameWidth(frameWidth);
    m_inGrabber->setFrameWidth(frameWidth);
    m_outGrabber->setFrameWidth(frameWidth);
    m_currentGrabber->setFrameWidth(frameWidth);

    updateWidth();

    if (m_parentScrollArea && m_autoScroll)
        m_parentScrollArea->ensureVisible((m_currentGrabber->getFrameIndex() - m_startGrabber->getFrameIndex()) * (m_frameWidth - 1) + GrabberWidget::Width - 1, 0);

    update();
}


///
/// Protected Event Handlers
///


//!
//! Event handler that reacts to paint events.
//!
//! \param event The object containing details about the event.
//!
void TimelineWidget::paintEvent ( QPaintEvent *event )
{
    //Log::debug(QString("paintEvent handler has been called. Rect: (%1, %2, %3, %4)").arg(event->rect().left()).arg(event->rect().top()).arg(event->rect().width()).arg(event->rect().height()), "TimelineWidget::paintEvent");

    paintImage(event->rect());

    QPainter painter (this);
    painter.drawImage(QRect(event->rect().topLeft(), m_image.size()), m_image);
}


//!
//! Event handler that reacts to key press events.
//!
//! \param event The object containing details about the event.
//!
void TimelineWidget::keyPressEvent ( QKeyEvent *event )
{
    int key = event->key();
    bool inEditingMode = m_currentFrameEdit->isVisible();
    if (inEditingMode) {
        if (key == Qt::Key_Return || key == Qt::Key_Enter) {
            // convert the entered index text to a numeric value
            QString indexText = m_currentFrameEdit->text();
            bool ok = true;
            int index = indexText.toInt(&ok);
            if (ok) {
                // limit the index by the in and out frame indices
                int inFrameIndex = m_inGrabber->getFrameIndex();
                int outFrameIndex = m_outGrabber->getFrameIndex();
                if (index < inFrameIndex)
                    index = inFrameIndex;
                else if (index > outFrameIndex)
                    index = outFrameIndex;
                // stop editing the current frame index
                editCurrentFrame(false);
                // notify connected objects that the index of the current frame has changed
                emit currentFrameSet(index);
            } else
                Log::error("The entered index is not a number.", "TimelineWidget::on_m_currentFrameEdit_returnPressed");
        } else if (key == Qt::Key_Escape)
            editCurrentFrame(false);
        return;
    }

    switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            editCurrentFrame();
            return;
    }

    QWidget::keyPressEvent(event);
}


//!
//! Event handler that reacts to mouse double clicks.
//!
//! \param event The object containing details about the event.
//!
void TimelineWidget::mouseDoubleClickEvent ( QMouseEvent *event )
{
    if (event->buttons() & Qt::LeftButton)
        // check if the mouse cursor is over the current frame grabber
        if (grabberAt(event->pos(), false) == m_currentGrabber)
            editCurrentFrame();
}


//!
//! Event handler that reacts to mouse movement.
//!
//! \param event The object containing details about the event.
//!
void TimelineWidget::mouseMoveEvent ( QMouseEvent *event )
{
    if (event->buttons() == Qt::NoButton) {
        // check if the mouse cursor is over one of the range grabbers
        if (grabberAt(event->pos()) != 0)
            setCursor(Qt::SizeHorCursor);
        else
            setCursor(Qt::ArrowCursor);
    } else if (event->buttons() & Qt::LeftButton) {
        // calculate the index of the clicked frame
        int frameWidth = m_frameWidth - 1;
        int x = event->x() + m_startGrabber->getFrameIndex() * frameWidth - (GrabberWidget::Width - 1);
        int index = x / frameWidth - (x < 0);

        // get indices of grabber frames
        int startFrameIndex = m_startGrabber->getFrameIndex();
        int endFrameIndex = m_endGrabber->getFrameIndex();
        int inFrameIndex = m_inGrabber->getFrameIndex();
        int outFrameIndex = m_outGrabber->getFrameIndex();
        int currentFrameIndex = m_currentGrabber->getFrameIndex();

        if (!m_movedGrabber) {
            // limit the current frame index by the in and out frame indices
            if (index < inFrameIndex)
                index = inFrameIndex;
            else if (index > outFrameIndex)
                index = outFrameIndex;

            // check if the current frame index has changed
            if (index != currentFrameIndex)
                emit currentFrameSet(index);
        } else if (m_movedGrabber == m_inGrabber) {
            // limit the in frame index by the out frame index
            int inIndex = index + 1;
            if (inIndex > outFrameIndex)
                inIndex = outFrameIndex;

            // check if the in frame index has changed
            if (inIndex != inFrameIndex) {
                // check if the in frame index has exceeded the start frame index
                if (inIndex < startFrameIndex)
                    emit startFrameSet(inIndex);
                emit inFrameSet(inIndex);
            }
        } else if (m_movedGrabber == m_outGrabber) {
            // limit the out frame index by the in frame index
            int outIndex = index - 1;
            if (outIndex < inFrameIndex)
                outIndex = inFrameIndex;

            // check if the out frame index has changed
            if (outIndex != outFrameIndex) {
                // check if the out frame index has exceeded the end frame index
                if (outIndex > endFrameIndex)
                    emit endFrameSet(outIndex);
                emit outFrameSet(outIndex);
            }
        } else if (m_movedGrabber == m_startGrabber) {
            // limit the start frame index by the out frame index
            int startIndex = index + 1;
            if (startIndex > outFrameIndex)
                startIndex = outFrameIndex;

            // check if the start frame index has changed
            if (startIndex != startFrameIndex)
                emit startFrameSet(startIndex);
        } else if (m_movedGrabber == m_endGrabber) {
            // limit the end frame index by the in frame index
            int endIndex = index - 1;
            if (endIndex < inFrameIndex)
                endIndex = inFrameIndex;

            // check if the end frame index has changed
            if (endIndex != endFrameIndex)
                emit endFrameSet(endIndex);
        }
    }
}


//!
//! Event handler that reacts to mouse clicks.
//!
//! \param event The object containing details about the event.
//!
void TimelineWidget::mousePressEvent ( QMouseEvent *event )
{
    // leave the editing mode for the current frame if it is currently being editing
    if (m_currentFrameEdit->isVisible()) {
        editCurrentFrame(false);
        update();
    }

    // skip the rest of the function if a different button than the left mouse button has been clicked
    if (event->button() != Qt::LeftButton)
        return;

    // find out if a grabber has been clicked
    m_movedGrabber = grabberAt(event->pos());
    if (m_movedGrabber)
        update();
    else {
        // calculate the index of the clicked frame
        int frameWidth = m_frameWidth - 1;
        int x = event->x() + m_startGrabber->getFrameIndex() * frameWidth - (GrabberWidget::Width - 1);
        int index = x / frameWidth - (x < 0);

        // limit the current frame index by the in and out frame indices
        if (index < m_inGrabber->getFrameIndex())
            index = m_inGrabber->getFrameIndex();
        else if (index > m_outGrabber->getFrameIndex())
            index = m_outGrabber->getFrameIndex();

        // check if the current frame index has changed
        if (index != m_currentGrabber->getFrameIndex())
            emit currentFrameSet(index);
    }
}


//!
//! Event handler that reacts to the release of mouse buttons.
//!
//! \param event The object containing details about the event.
//!
void TimelineWidget::mouseReleaseEvent ( QMouseEvent *event )
{
    if (m_movedGrabber) {
        m_movedGrabber = 0;
        update();
    }
}


///
/// Private Functions
///


//!
//! Updates the width of the timeline widget depending on the current start
//! and end frames and the current frame width.
//!
void TimelineWidget::updateWidth ()
{
    int w = (m_endGrabber->getFrameIndex() - m_startGrabber->getFrameIndex() + 1) * (m_frameWidth - 1) + 1 + GrabberWidget::Width + GrabberWidget::Width - 2;
    if (w < width())
        w += 10;
    setFixedWidth(w);
}


//!
//! Paints a part of the image that is drawn when painting the widget.
//!
//! \param rectangle The area of the image to paint.
//!
void TimelineWidget::paintImage ( const QRect &rectangle )
{
    int horizontalOffset = rectangle.left();
    int w = rectangle.width();
    int h = rectangle.height();

    // get frame indices from grabbers
    int startFrame = m_startGrabber->getFrameIndex();
    int endFrame = m_endGrabber->getFrameIndex();
    int inFrame = m_inGrabber->getFrameIndex();
    int outFrame = m_outGrabber->getFrameIndex();

    // calculate dimensions
    int frameWidth = m_frameWidth - 1;
    int frameHeight = m_frameHeight - 1;
    int grabberWidth = GrabberWidget::Width - 1;
    QSize frameSize = QSize(frameWidth, frameHeight);

    // define colors
    QColor buttonColor (palette().color(QPalette::Button));
    QColor midColor (palette().color(QPalette::Mid));
    QColor baseColor (palette().color(QPalette::Base));
    QColor windowColor (palette().color(QPalette::Window));
    QColor buttonTextColor (palette().color(QPalette::ButtonText));
    QColor buttonAlphaColor (buttonColor);
    buttonAlphaColor.setAlpha(200);

    // (re-)create the image
    m_image = QImage(rectangle.size(), QImage::Format_RGB32);
    m_image.fill(windowColor.rgb());

    // start painting on the image
    QPainter painter (&m_image);

    // draw horizontal frame borders
    int rangeWidth = grabberWidth + (endFrame - startFrame + 1) * frameWidth + grabberWidth;
    if (rangeWidth > w)
        rangeWidth = w;
    painter.setPen(buttonColor);
    painter.drawLine(QPoint(0, 0), QPoint(rangeWidth, 0));
    painter.drawLine(QPoint(0, frameHeight), QPoint(rangeWidth, frameHeight));

    // draw frame range background
    painter.setBrush(QBrush(baseColor));
    painter.setPen(midColor);
    QRect frameRangeRect (-1, 0, w, frameHeight);
    int inFrameX = (inFrame - startFrame) * frameWidth;
    int outFrameX = (outFrame - startFrame + 1) * frameWidth;
    if (inFrameX >= horizontalOffset) {
        frameRangeRect.setLeft(inFrameX - horizontalOffset);
        if (outFrameX - inFrameX + grabberWidth < w)
            frameRangeRect.setWidth(outFrameX - inFrameX + grabberWidth);
    } else
        if (outFrameX - horizontalOffset + grabberWidth < w)
            frameRangeRect.setWidth(outFrameX - horizontalOffset + grabberWidth);
    if (frameRangeRect.left() < w && frameRangeRect.width() > 0)
        painter.drawRect(frameRangeRect.adjusted(0, 0, 1, 0));

    // set the initial pen and highlight colors
    QColor penColor (buttonColor);
    QColor highlightColor (windowColor.darker(102));
    if (inFrameX <= horizontalOffset && outFrameX >= horizontalOffset) {
        penColor = midColor;
        highlightColor = baseColor.darker(104);
    }

    // calculate indices of first and last frame to display in the widget
    int startIndex = horizontalOffset / frameWidth + startFrame - 1;
    int endIndex = startIndex + (rangeWidth - grabberWidth - grabberWidth) / frameWidth + 2;

    // iterate over the frame range and draw the frame rectangles and index texts
    for (int index = startIndex; index <= endIndex; ++index) {
        int x = grabberWidth - horizontalOffset % frameWidth + (index - startIndex - 1) * frameWidth;

        // set pen and highlight colors according to frame index
        if (index == inFrame) {
            penColor = midColor;
            highlightColor = baseColor.darker(104);
        } else if (index == outFrame + 1) {
            penColor = buttonColor;
            highlightColor = windowColor.darker(102);
        }

        // highlight every n-th frame rectangle by drawing it filled
        if (index == 1 || index != 0 && m_tickStep > 1 && index % m_tickStep == 0) {
            painter.setBrush(highlightColor);
            painter.setPen(Qt::NoPen);
            painter.drawRect(QRect(QPoint(x, 1), frameSize + QSize(1, -1)));
        }

        // draw frame rectangle separator line
        painter.setPen(QPen(QBrush(penColor), 1, Qt::DotLine));
        painter.drawLine(QPoint(x, -1), QPoint(x, frameHeight));

        // draw index text
        if (((index == 1 || index != 0 && index % m_tickStep == 0) || (m_frameWidth > 35)) && index <= endFrame) {
            QString indexText = QString("%1").arg(index);
            QSize indexTextSize (painter.fontMetrics().size(0, indexText) + QSize(4, 0));
            QRect indexTextRect (QPoint(x + m_frameWidth / 2 - indexTextSize.width() / 2, m_frameHeight + 2), indexTextSize);
            painter.drawText(indexTextRect.adjusted(1, 1, 0, 0), Qt::AlignCenter, indexText);
        }
    }

    // set the frame index to display below the corresponding frame rectangle
    int indexToDisplay;
    if (m_movedGrabber)
        indexToDisplay = m_movedGrabber->getFrameIndex();
    else
        indexToDisplay = m_currentGrabber->getFrameIndex();

    // calculate the index rectangle and text
    int frameRectangleX = grabberWidth + (indexToDisplay - startFrame) * frameWidth - horizontalOffset;
    QString indexText (QString("%1").arg(indexToDisplay));
    QSize indexTextSize (painter.fontMetrics().size(0, indexText));
    QRect indexTextRect (QPoint(frameRectangleX + (frameWidth -indexTextSize.width()) / 2 - 2, frameHeight + 4), indexTextSize + QSize(4, -1));

    // set the font to use for the index text
    QFont font (painter.font());
    font.setBold(true);

    // draw the index frame rectangle and text
    painter.setBrush(QBrush(buttonAlphaColor));
    painter.setPen(midColor);
    painter.drawRect(indexTextRect);
    painter.setPen(buttonTextColor);
    painter.setFont(font);
    painter.drawText(indexTextRect.adjusted(1, 1, 0, 0), Qt::AlignCenter, indexText);
}


//!
//! Returns the grabber at the given mouse pointer position.
//!
//! \param position The position of the mouse pointer over the widget.
//! \param rangeGrabbersOnly Flag to control whether to only check if a frame range grabber is located at the given position.
//! \return The grabber at the given mouse pointer position.
//!
GrabberWidget * TimelineWidget::grabberAt ( const QPoint &position, bool rangeGrabbersOnly /* = true */ )
{
    GrabberWidget *result = 0;

    int x = position.x();
    if (x >= m_inGrabber->x() - 2 && x <= m_inGrabber->x() + GrabberWidget::Width)
        result = m_inGrabber;
    else if (x >= m_outGrabber->x() - 1 && x <= m_outGrabber->x() + GrabberWidget::Width + 1)
        result = m_outGrabber;
    else if (x >= m_startGrabber->x() - 2 && x <= m_startGrabber->x() + GrabberWidget::Width)
        result = m_startGrabber;
    else if (x >= m_endGrabber->x() - 1 && x <= m_endGrabber->x() + GrabberWidget::Width + 1)
        result = m_endGrabber;

    if (!result && !rangeGrabbersOnly && x >= m_currentGrabber->x() - 2 && x <= m_currentGrabber->x() + m_currentGrabber->width() + 2)
        result = m_currentGrabber;

    return result;
}


//!
//! Enters or leaves the mode for editing the index of the current frame.
//!
//! \param startEditing Flag to control whether to start or stop editing the current frame index.
//!
void TimelineWidget::editCurrentFrame ( bool startEditing /* = true */ )
{
    if (startEditing) {
        m_currentFrameEdit->setText(QString("%1").arg(m_currentGrabber->getFrameIndex()));
        m_currentFrameEdit->move(m_currentGrabber->x() + (m_currentGrabber->width() - m_currentFrameEdit->width()) / 2, m_currentGrabber->y() + m_currentGrabber->height() + 2);
        m_currentFrameEdit->selectAll();
        m_currentFrameEdit->setVisible(true);
        m_currentFrameEdit->setFocus();
    } else {
        m_currentGrabber->setFocus();
        m_currentFrameEdit->setVisible(false);
    }
}


///
/// Private Slots
///


//!
//! Slot that is called when the return key has been pressed while the
//! current frame line edit has focus.
//!
//! Sets the entered index as the new index for the current frame.
//!
void TimelineWidget::on_m_currentFrameEdit_returnPressed ()
{
}

} // end namespace Frapper
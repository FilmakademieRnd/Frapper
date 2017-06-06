/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//!
//! \file "TextRender.h"
//! \brief Header file for TextRender class.
//!
//! \author     Michael Buﬂler <michael.bussler@filmakademie.de>
//! \version    0.1
//! \date       07.02.2012 (last updated)
//!

#ifndef TEXTRENDER_H
#define TEXTRENDER_H

#include "OgreTools.h"
#include "ImageNode.h"

#include <QImage>

// OGRE
#include <Ogre.h>
#if (OGRE_PLATFORM  == OGRE_PLATFORM_WIN32)
#include <windows.h>
#endif

namespace TextRenderNode {
using namespace Frapper;

//!
//! Class for a stereo renderer.
//!
class TextRenderNode : public ImageNode
{

    Q_OBJECT

public: // constructors and destructors

    //!
    //! Constructor of the TextRender class.
    //!
    //! \param name The name for the new node.
    //! \param parameterRoot A copy of the parameter tree specific for the type of the node.
    //!
    TextRenderNode ( const QString &name, ParameterGroup *parameterRoot );

    //!
    //! Destructor of the TextRender class.
    //!
    //! Defined virtual to guarantee that the destructor of a derived class
    //! will be called if the instance of the derived class is saved in a
    //! variable of its parent class type.
    //!
    virtual ~TextRenderNode ();

public:

private slots: //
    
    void updateOutputImage();

    void resolutionChanged();

    void fontChanged();

private: // functions

    QImage m_qImage;
    QFont m_font;
    QColor m_color;

    Ogre::TexturePtr m_outputImage;

    int m_fontsize;
    int m_width, m_height;


protected: // data

};

} // namespace TextRenderNode 

#endif
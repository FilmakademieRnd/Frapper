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
//! \file "PainterUndoCommands.h"
//! \brief Header file for PainterUndoCommands Classes, representing different kind of undo/redo scenarios
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \version    1.0
//! \date       23.01.2014 (last updated)
//!

#ifndef PAINTERUNDOCOMMANDS_H
#define PAINTERUNDOCOMMANDS_H

#include <QUndoCommand>
#include "PainterGraphicsView.h"
#include "PainterPanelNode.h"
#include "PainterPanel.h"

namespace PainterPanel {
	using namespace Frapper;

	//!
	//! Class representing a Command for Undo/Redo of selecting a different PainterPanel Tool
	//!
	class ToolSelectCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the ToolSelectCommand class.
		//!
		ToolSelectCommand ( PainterPanel* parent, PainterGraphicsView::DrawMode oldMode, PainterGraphicsView::DrawMode newMode, QString name);

		//!
		//! Destructor of the ToolSelectCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~ToolSelectCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

	// data
	private:
		PainterPanel* m_parent;
		PainterGraphicsView::DrawMode m_oldMode;
		PainterGraphicsView::DrawMode m_newMode;

	};

	//!
	//! Class representing a Command for changing the pen size in the PainterPanel
	//!
	class PenSizeCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the PenSizeCommand class.
		//!
		PenSizeCommand ( PainterPanel* parent, int oldSize, int newSize, QString name);

		//!
		//! Destructor of the PenSizeCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~PenSizeCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		PainterPanel* m_parent;
		int m_oldSize;
		int m_newSize;

	};

	//!
	//! Class representing a Command for changing the pen color in the PainterPanel
	//!
	class PenColorCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the PenColorCommand class.
		//!
		PenColorCommand ( PainterPanel* parent, int oldColor, int newColor, QString name);

		//!
		//! Destructor of the PenColorCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~PenColorCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		PainterPanel* m_parent;
		int m_oldColor;
		int m_newColor;

	};

	//!
	//! Class representing a Command for changing the pen opacity in the PainterPanel
	//!
	class PenOpacityCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the PenOpacityCommand class.
		//!
		PenOpacityCommand ( PainterPanel* parent, int oldOpacity, int newOpacity, QString name);

		//!
		//! Destructor of the PenOpacityCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~PenOpacityCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		PainterPanel* m_parent;
		int m_oldOpacity;
		int m_newOpacity;

	};

	//!
	//! Class representing a Command for changing the fill color in the PainterPanel
	//!
	class FillColorCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the FillColorCommand class.
		//!
		FillColorCommand ( PainterPanel* parent, int oldColor, int newColor, QString name);

		//!
		//! Destructor of the FillColorCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~FillColorCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		PainterPanel* m_parent;
		int m_oldColor;
		int m_newColor;

	};

	//!
	//! Class representing a Command for changing the fill status in the PainterPanel
	//!
	class FillToogleCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the FillToogleCommand class.
		//!
		FillToogleCommand (PainterPanel* parent, bool value, QString name);

		//!
		//! Destructor of the FillToogleCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~FillToogleCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		PainterPanel* m_parent;
		bool m_value;
	};

	//!
	//! Class representing a Command for creating a item and undo that.
	//!
	class CreateItemCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the CreateItemCommand class.
		//!
		CreateItemCommand (PainterPanel* ui, QPointF pos, BaseShapeItem::ItemType type);

		//!
		//! Destructor of the CreateItemCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~CreateItemCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		PainterPanel* m_ui;
		QString m_name;
		QPointF m_pos;
		BaseShapeItem::ItemType m_type;
	};

	//!
	//! Class representing a Command for deleting a item and undo that.
	//!
	class DeleteItemCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the DeleteItemCommand class.
		//!
		DeleteItemCommand(PainterPanel* ui, QList<QString> items, int time);

		//!
		//! Destructor of the DeleteItemCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~DeleteItemCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		QList<ParameterGroup*> m_parameters;
		PainterPanel* m_ui;
		QList<QString> m_items;
		int m_time;
	};

	//!
	//! Class representing a Command for moving a item and undo that.
	//!
	class MoveItemCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the DeleteItemCommand class.
		//!
		MoveItemCommand(PainterPanel* ui, QList<QString> items, QPointF pos1, QPointF pos2, int time);

		//!
		//! Destructor of the DeleteItemCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~MoveItemCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		QPointF m_oldPosition;
		QPointF m_distance;
		PainterPanel* m_ui;
		QList<QString> m_items;
		int m_time;
	};

	//!
	//! Class representing a Command for moving a item and undo that.
	//!
	class RotateItemCommand : public QUndoCommand
	{

	public: // constructors and destructors

		//!
		//! Constructor of the DeleteItemCommand class.
		//!
		RotateItemCommand(PainterPanel* ui, QList<QString> items, qreal oldRot, qreal newRot, int time);

		//!
		//! Destructor of the DeleteItemCommand class.
		//!
		//! Defined virtual to guarantee that the destructor of a derived class
		//! will be called if the instance of the derived class is saved in a
		//! variable of its parent class type.
		//!
		virtual ~RotateItemCommand();

		//! undo the command
		void undo();

		//! redo/apply the command
		void redo();

		// data
	private:
		qreal m_savedRotation;
		qreal m_newRotation;
		PainterPanel* m_ui;
		QList<QString> m_items;
		int m_time;
	};

} // end namespace PainterPanel

#endif // PAINTERUNDOCOMMANDS_H
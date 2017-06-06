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
//! \file "PainterUndoCommands.pp"
//! \brief Implementation File for PainterUndoCommands Classes, representing different kind of undo/redo scenarios
//!
//! \author     Stefan Seibert <stefan.seibert@filmakademie.de>
//! \version    1.0
//! \date       23.01.2014 (last updated)
//!

#include "PainterUndoCommands.h"

namespace PainterPanel {
	using namespace Frapper;

	ToolSelectCommand::ToolSelectCommand(PainterPanel* parent, PainterGraphicsView::DrawMode oldMode, PainterGraphicsView::DrawMode newMode, QString name) 
		: QUndoCommand(name),
		m_parent(parent),
		m_oldMode(oldMode),
		m_newMode(newMode)
	{}

	ToolSelectCommand::~ToolSelectCommand()
	{}

	void ToolSelectCommand::undo() {
		m_parent->changeDrawMode(m_oldMode);
	}

	void ToolSelectCommand::redo(){
		m_parent->changeDrawMode(m_newMode);
	}

	PenSizeCommand::PenSizeCommand(PainterPanel* parent, int oldSize, int newSize, QString name) 
		: QUndoCommand(name),
		m_parent(parent),
		m_oldSize(oldSize),
		m_newSize(newSize)
	{}

	PenSizeCommand::~PenSizeCommand()
	{}

	void PenSizeCommand::undo() {
		m_parent->changingPenSize(m_oldSize);
	}

	void PenSizeCommand::redo(){
		m_parent->changingPenSize(m_newSize);
	}

	PenColorCommand::PenColorCommand(PainterPanel* parent, int oldColor, int newColor, QString name) 
		: QUndoCommand(name),
		m_parent(parent),
		m_oldColor(oldColor),
		m_newColor(newColor)
	{}

	PenColorCommand::~PenColorCommand()
	{}

	void PenColorCommand::undo() {
		m_parent->changingPenColor(m_oldColor);
	}

	void PenColorCommand::redo(){
		m_parent->changingPenColor(m_newColor);
	}

	PenOpacityCommand::PenOpacityCommand(PainterPanel* parent, int oldOpacity, int newOpacity, QString name) 
		: QUndoCommand(name),
		m_parent(parent),
		m_oldOpacity(oldOpacity),
		m_newOpacity(newOpacity)
	{}

	PenOpacityCommand::~PenOpacityCommand()
	{}

	void PenOpacityCommand::undo() {
		m_parent->changingPenOpacity(m_oldOpacity);
	}

	void PenOpacityCommand::redo(){
		m_parent->changingPenOpacity(m_newOpacity);
	}

	FillColorCommand::FillColorCommand(PainterPanel* parent, int oldColor, int newColor, QString name) 
		: QUndoCommand(name),
		m_parent(parent),
		m_oldColor(oldColor),
		m_newColor(newColor)
	{}

	FillColorCommand::~FillColorCommand()
	{}

	void FillColorCommand::undo() {
		m_parent->changingFillColor(m_oldColor);
	}

	void FillColorCommand::redo(){
		m_parent->changingFillColor(m_newColor);
	}

	FillToogleCommand::FillToogleCommand(PainterPanel* parent, bool value, QString name) 
		: QUndoCommand(name),
		m_parent(parent),
		m_value(value)
	{}

	FillToogleCommand::~FillToogleCommand()
	{}

	void FillToogleCommand::undo() {
		m_parent->changingFillStatus(!m_value);
	}

	void FillToogleCommand::redo(){
		m_parent->changingFillStatus(m_value);
	}

	CreateItemCommand::CreateItemCommand(PainterPanel* ui, QPointF pos, BaseShapeItem::ItemType type) 
		: QUndoCommand("Item Creation"),
		m_ui(ui),
		m_pos(pos),
		m_type(type)
	{
		m_name = "";
	}

	CreateItemCommand::~CreateItemCommand()
	{}

	void CreateItemCommand::undo() {
		if(m_ui) {
			m_ui->removeItem(m_name);
		}
	}

	void CreateItemCommand::redo() {
		if(m_ui) {

			if(m_name == "")
				m_name = m_ui->addItem(m_pos, m_type, m_name);

			else
				m_ui->addItem(m_pos, m_type, m_name);

			this->setText( m_name + " Creation");
		}
	}

	DeleteItemCommand::DeleteItemCommand(PainterPanel* ui, QList<QString> items, int time) 
		: QUndoCommand("Item Deletion"),
		m_ui(ui),
		m_items(items),
		m_time(time)
	{
		QList<ParameterGroup*> m_parameters = QList<ParameterGroup*>();
	}

	DeleteItemCommand::~DeleteItemCommand()
	{}

	void DeleteItemCommand::undo() {

		if(m_ui && m_parameters.length() > 0) {
		
			ParameterGroup* nodeGroup = m_ui->getItemParameterGroup();

			if(nodeGroup) {

				foreach(ParameterGroup* item, m_parameters) {

					// make copy from parameter@command
					ParameterGroup* newParameter = new ParameterGroup(*item);

					if(newParameter) 
					{
						// add the copy into the node
						nodeGroup->addParameter(newParameter);

						// create shape from data
						PainterPanelItemData data = PainterPanelItemData(newParameter, m_time);

						// data valid? make shape and history entry
						if(data.Valid())
							m_ui->recreateItem(data);
					}
					// delete the parameter in the command
					delete item;
				}
				// clear command data structure
				m_parameters.clear();
			}
		}
	}

	void DeleteItemCommand::redo() {

		if(m_ui && m_items.length() > 0) {
		
			ParameterGroup* nodeGroup = m_ui->getItemParameterGroup();

			if(nodeGroup) {

				foreach(QString name, m_items) {

					// make copy from parameter@node
					ParameterGroup* itemParameter = nodeGroup->getParameterGroup( name, false );

					if(itemParameter)
					{
						// add the copy into the command data structure
						ParameterGroup* newParameter = new ParameterGroup(*itemParameter);
						m_parameters.append(newParameter);

						// remove original
						nodeGroup->removeParameterGroup( name );
					}
					// remove shape and history entry
					m_ui->removeItem(name);
				}
			}
		}
	}

	MoveItemCommand::MoveItemCommand(PainterPanel* ui, QList<QString> items, QPointF distance, QPointF oldPosition, int time) 
		: QUndoCommand("Item Moving"),
		m_ui(ui),
		m_items(items),
		m_time(time),
		m_distance(distance),
		m_oldPosition(oldPosition)
	{}

	MoveItemCommand::~MoveItemCommand()
	{}

	void MoveItemCommand::undo() {

		if(m_ui && m_items.length() > 0) {

			foreach(QString name, m_items) {

				BaseShapeItem* item = m_ui->getGraphicsItemByName(name);

				if(item)
					item->setPosition(m_oldPosition);
			}
		}
	}

	void MoveItemCommand::redo() {

		if(m_ui && m_items.length() > 0) {

			foreach(QString name, m_items) {

				BaseShapeItem* item = m_ui->getGraphicsItemByName(name);

				if(item) {

					if(item)
						item->setPosition(m_oldPosition+m_distance);
				}
			}
		}
	}


	RotateItemCommand::RotateItemCommand(PainterPanel* ui, QList<QString> items, qreal oldRot, qreal newRot, int time)
		: QUndoCommand("Item Rotation"),
		m_ui(ui),
		m_items(items),
		m_savedRotation(oldRot),
		m_newRotation(newRot),
		m_time(time)
	{}

	RotateItemCommand::~RotateItemCommand()
	{}

	void RotateItemCommand::undo() {

		if(m_ui && m_items.length() > 0) {

			foreach(QString name, m_items) {

				BaseShapeItem* item = m_ui->getGraphicsItemByName(name);

				if(item)
					item->setRotation(m_savedRotation);
			}
		}
	}

	void RotateItemCommand::redo() {

		if(m_ui && m_items.length() > 0) {

			foreach(QString name, m_items) {

				BaseShapeItem* item = m_ui->getGraphicsItemByName(name);

				if(item) {

					if(item)
						item->setRotation(m_newRotation);
				}
			}
		}
	}

} // namespace PainterPanel
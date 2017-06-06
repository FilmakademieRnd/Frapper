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
//! \file "Log.h"
//! \brief Import header file for Log class.
//!
//! \author     Stefan Habel <stefan.habel@filmakademie.de>
//! \author     Nils Zweiling <nils.zweiling@filmakademie.de>
//! \version    1.0
//! \date       29.06.2009 (last updated)
//!

#ifndef LOG_H
#define LOG_H

#include "FrapperPrerequisites.h"

#include <iostream>
#include <streambuf>

#include <QTableView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QIcon>

namespace Frapper {

	class StreamListener;

//!
//! Static class for managing log messages.
//!
class FRAPPER_CORE_EXPORT Log
{

public: // nested enumerations

    //!
    //! Nested enumeration for the different types of log messages.
    //!
    enum MessageType {
        MT_Info = 1,
        MT_Warning = 2,
        MT_Error = 4,
        MT_Debug = 8,
        MT_Qt = 16,
        MT_Ogre = 32
    };

private: // type definitions

    //!
    //! Type definition for a map of message models by message type mask.
    //!
    typedef QMap<int, QStandardItemModel *> MessageModelMap;

    //!
    //! Type definition for a map of proxy models by message type mask.
    //!
    typedef QMap<int, QSortFilterProxyModel *> ProxyModelMap;

    //!
    //! Type definition for a map of log message types along with their activation
    //! values.
    //!
    typedef QMap<Log::MessageType, bool> MessageFilters;

    //!
    //! Type definition for a boolean value stating whether message filtering is
    //! enabled at all and the filters for the individual log message types.
    //!
    typedef QPair<bool, MessageFilters> FilterSettings;

    //!
    //! Type definition for the map of table views registered with the log message
    //! handler along with their filter settings.
    //!
    typedef QMap<QTableView *, FilterSettings> TableViewMap;

public: // static functions

    //!
    //! Initializes private static data of the log message handler.
    //!
    //! \param useStandardStreams Flag to control whether to also output log messages to stdout and stderr.
    //!
    static void initialize ( bool useStandardStreams );

    //!
    //! Frees all resources that were used by private static data of the log
    //! message handler.
    //!
    static void finalize ();

    //!
    //! Loads the icons for the different log message types.
    //!
    static void loadIcons ();

    //!
    //! Registers the given table view as an output widget for log messages.
    //!
    //! \param tableView The table view to register as an output widget.
    //!
    static void registerView ( QTableView *tableView );

    //!
    //! Unregisters the given table view from the log message handler.
    //!
    //! \param tableView The table view to unregister from the log message handler.
    //!
    static void unregisterView ( QTableView *tableView );

    //!
    //! Adds the given text to the list of informational log messages.
    //!
    //! \param message The text to add as an informational log message.
    //! \param function The signature of the function that is reporting the message.
    //!
    static void info ( const QString &message, const QString &function = "" );

    //!
    //! Adds the given text to the list of warning log messages.
    //!
    //! \param message The text to add as a warning log message.
    //! \param function The signature of the function that is reporting the message.
    //!
    static void warning ( const QString &message, const QString &function = "" );

    //!
    //! Adds the given text to the list of error log messages.
    //!
    //! \param message The text to add as an error log message.
    //! \param function The signature of the function that is reporting the message.
    //!
    static void error ( const QString &message, const QString &function = "" );

    //!
    //! Adds the given text to the list of debugging log messages.
    //!
    //! \param message The text to add as a debugging log message.
    //! \param function The signature of the function that is reporting the message.
    //!
    static void debug ( const QString &message, const QString &function = "" );

    //!
    //! Adds a message to the list of warning log messages that the selected
    //! function is not available yet.
    //!
    //! \param function The signature of the function that is reporting the message.
    //!
    static void na ( const QString &function = "" );

    //!
    //! Adds the given text to the list of Qt log messages.
    //!
    //! \param qtMessageType The type of the Qt message.
    //! \param qtMessage The message text.
    //!
#if QT_VERSION >= 0x050000
	static void Log::addQtLogMessage(QtMsgType qtMessageType, const QMessageLogContext &context, const QString &qtMessage);
#else
	static void addQtLogMessage(QtMsgType qtMessageType, const char *qtMessage);
#endif
	
    //!
    //! Adds the given text to the list of OGRE log messages.
    //!
    //! \param message The text to add as an OGRE log message.
    //! \param logMessageLevel The importance of the logged message.
    //!
    static void addOgreLogMessage ( const QString &message, const QString &logMessageLevel );

    //!
    //! Toggles filtering of log messages for the given table view.
    //!
    //! \param tableView The table view to enable or disable filtering for.
    //! \param enabled Flag that controls whether to enable or disable filtering.
    //!
    static void setFilterEnabled ( QTableView *tableView, bool enabled );

    //!
    //! Toggles a specific type of log messages in the given table view by
    //! making these messages visible or invisible.
    //!
    //! \param tableView The table view to filter messages in.
    //! \param type The type of log messages to filter.
    //! \param visible Flag that controls whether to hide or show the messages.
    //!
    static void filter ( QTableView *tableView, MessageType type, bool visible );

    //!
    //! Filters log messages in the given table view by
    //! using a reg expression.
    //!
    //! \param searchText The search text.
    //!
    static void filter ( QTableView *tableView, const QString &searchText);

    //!
    //! Clears the history of log messages.
    //!
    static void clear ();

    //!
    //! Builds a string representing the message with the given row index in
    //! the given item model.
    //!
    //! \return A string representing the message with the given row index in the given item model.
    //!
    static QString getMessage ( QStandardItemModel *model, int row );

	//!
	//! Updates the last log message with the given text.
	//!
	//! \param messageType The type of log message to add.
	//! \param message The text to add as a log message.
	//! \param function The signature of the function that is reporting the message.
	//!
	static void updateMessage ( MessageType messageType, const QString &message, const QString &function = "" );

private: // static functions

    //!
    //! Adds the given text to the list of log messages.
    //!
    //! \param messageType The type of log message to add.
    //! \param message The text to add as a log message.
    //! \param function The signature of the function that is reporting the message.
    //!
    static void addMessage ( MessageType messageType, const QString &message, const QString &function = "" );

    //!
    //! Fills the model that is used in the given table view with messages of types
    //! according to the active view filters.
    //!
    //! \param tableView The table view whose model to fill with log messages.
    //!
    static void updateModel ( QTableView *tableView );

    //!
    //! Writes the given message preceeded by the given header text and the
    //! given function name (if available) to the standard output stream.
    //!
    //! \param header The text to put before the message.
    //! \param message The message to write to the standard output stream.
    //! \param function The name of a function that reported the message.
    //!
    static void printToStandardOutput ( const QString &header, const QString &message, const QString &function );

    //!
    //! Writes the given message preceeded by the given header text and the
    //! given function name (if available) to the standard error stream.
    //!
    //! \param header The text to put before the message.
    //! \param message The message to write to the standard error stream.
    //! \param function The name of a function that reported the message.
    //!
    static void printToStandardError ( const QString &header, const QString &message, const QString &function );
	
public: // static data

	//!
	//! static stream listener to pipe std out to frapper log
	//!
	static StreamListener s_streamListener;

private: // static data

    //!
    //! Flag that states whether log messages should also be written to stdout
    //! and stderr.
    //!
    static bool s_useStandardStreams;

    //!
    //! Flag that states whether the log message handler has been initialized.
    //!
    static bool s_initialized;

    //!
    //! The list of header labels for the standard item models.
    //!
    static QStringList s_headerLabels;

    //!
    //! The map of standard item models for all combinations of log message
    //! types.
    //!
    static MessageModelMap s_messageModels;

    //!
    //! The map of proxy models for all combinations of log message
    //! types.
    //!
    static ProxyModelMap s_proxyModels;

    //!
    //! The nested map for storing registered table views and their message
    //! filters.
    //!
    static TableViewMap s_tableViews;

    //!
    //! The list of icons for the different log message types.
    //!
    static QMap<MessageType, QIcon> s_messageIcons;

};

//!
//! Listen to messages on an ostream and print them to Frapper log
//!
//! \param stream the ostream to redirect, e.g. std::cout
//!

class FRAPPER_CORE_EXPORT StreamBuffer : public std::basic_streambuf<char>
{
public:
	StreamBuffer();
	void setName( const QString &name );

protected:
	virtual int_type overflow(int_type v);	
	virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
	std::string m_buffer;
	QString m_name;
};

class FRAPPER_CORE_EXPORT StreamListener : public std::basic_ostream< char, std::char_traits< char >>
{
public:
	StreamListener();
	void setName( QString name );

private:
	StreamBuffer buffer;
};

} // end namespace Frapper

#endif

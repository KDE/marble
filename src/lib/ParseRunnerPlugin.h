//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_PARSERUNNERPLUGIN_H
#define MARBLE_PARSERUNNERPLUGIN_H

#include <QtCore/QObject>
#include "PluginInterface.h"

namespace Marble
{

class MarbleAbstractRunner;

/**
  * A plugin for Marble to execute a parsing task.
  */
class MARBLE_EXPORT ParseRunnerPlugin : public QObject, public PluginInterface
{
    Q_OBJECT

public:
    /** Constructor with optional parent object */
    explicit ParseRunnerPlugin( QObject* parent = 0 );

    /** Destructor */
    virtual ~ParseRunnerPlugin();

    /**
     * @brief Returns the string that should appear in the user interface.
     *
     * For example, "Google Earth KML" should be returned for the kml parsing plugin.
     */
    virtual QString guiString() const = 0;

    /** Plugin factory method to create a new runner instance.
      * Method caller gets ownership of the returned object
      */
    virtual MarbleAbstractRunner* newRunner() const = 0;

    // Overridden methods with default implementations

    virtual QIcon icon() const;

private:
    class Private;
    Private *const d;
};

}

Q_DECLARE_INTERFACE( Marble::ParseRunnerPlugin, "org.kde.Marble.ParseRunnerPlugin/1.00" )

#endif // MARBLE_PARSERUNNERPLUGIN_H

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_PARSERUNNERPLUGIN_H
#define MARBLE_PARSERUNNERPLUGIN_H

#include <QObject>
#include "PluginInterface.h"

namespace Marble
{

class ParsingRunner;

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
    ~ParseRunnerPlugin() override;

    /**
      * Returns a short description of the supported file format.
      *
      * Example: "Google Earth KML"
      */
    virtual QString fileFormatDescription() const = 0;

    /**
      * Returns the file extensions associated with the file format.
      *
      * Example: "kml", "kmz"
      */
    virtual QStringList fileExtensions() const = 0;

    /** Plugin factory method to create a new runner instance.
      * Method caller gets ownership of the returned object
      */
    virtual ParsingRunner *newRunner() const = 0;

    // Overridden methods with default implementations

    QIcon icon() const override;

private:
    class Private;
    Private *const d;
};

}

Q_DECLARE_INTERFACE( Marble::ParseRunnerPlugin, "org.kde.Marble.ParseRunnerPlugin/1.01" )

#endif // MARBLE_PARSERUNNERPLUGIN_H

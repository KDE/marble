//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_SEARCHRUNNERPLUGIN_H
#define MARBLE_SEARCHRUNNERPLUGIN_H

#include <QtCore/QObject>
#include "PluginInterface.h"

namespace Marble
{

class SearchRunner;

/**
  * A plugin for Marble to execute a placemark search.
  */
class MARBLE_EXPORT SearchRunnerPlugin : public QObject, public PluginInterface
{
    Q_OBJECT

public:
    /** Constructor with optional parent object */
    explicit SearchRunnerPlugin( QObject* parent = 0 );

    /** Destructor */
    virtual ~SearchRunnerPlugin();

    /**
     * @brief Returns the string that should appear in the user interface.
     *
     * For example, "Nominatim" should be returned for the Nominatim search plugin.
     */
    virtual QString guiString() const = 0;

    /** Plugin factory method to create a new runner instance.
      * Method caller gets ownership of the returned object
      */
    virtual SearchRunner *newRunner() const = 0;

    /** True if the plugin supports its tasks on the given planet */
    bool supportsCelestialBody( const QString &celestialBodyId ) const;

    /** True if the plugin can execute its tasks without network access */
    bool canWorkOffline() const;

    /**
     * @brief Returns @code true @endcode if the plugin is able to perform its claimed task.
     *
     * The default implementation returns @code true @endcode. This method can be
     * overridden for example to indicate an incomplete installation.
     */
    virtual bool canWork() const;

    // Overridden methods with default implementations

    virtual QIcon icon() const;

protected:
    void setSupportedCelestialBodies( const QStringList &celestialBodies );

    void setCanWorkOffline( bool canWorkOffline );

private:
    class Private;
    Private *const d;
};

}

Q_DECLARE_INTERFACE( Marble::SearchRunnerPlugin, "org.kde.Marble.SearchRunnerPlugin/1.01" )

#endif // MARBLE_SEARCHRUNNERPLUGIN_H

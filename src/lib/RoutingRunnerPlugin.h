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

#ifndef MARBLE_ROUTINGRUNNERPLUGIN_H
#define MARBLE_ROUTINGRUNNERPLUGIN_H

#include <QtCore/QObject>
#include "PluginInterface.h"

#include <QtCore/QHash>
#include <QtGui/QWidget>

#include "routing/RoutingProfilesModel.h"

namespace Marble
{

class MarbleAbstractRunner;

/**
  * A plugin for Marble to execute a routing task.
  */
class MARBLE_EXPORT RoutingRunnerPlugin : public QObject, public PluginInterface
{
    Q_OBJECT

public:
    /** Constructor with optional parent object */
    explicit RoutingRunnerPlugin( QObject* parent = 0 );

    /** Destructor */
    virtual ~RoutingRunnerPlugin();

    /**
     * @brief Returns the string that should appear in the user interface.
     *
     * For example, "OpenRouteService" should be returned for the OpenRouteService routing plugin.
     */
    virtual QString guiString() const = 0;

    /** Plugin factory method to create a new runner instance.
      * Method caller gets ownership of the returned object
      */
    virtual MarbleAbstractRunner* newRunner() const = 0;

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

    /** A status message showing whether the plugin will be able to deliver results */
    QString statusMessage() const;

    class ConfigWidget : public QWidget
    {
    public:
        virtual void loadSettings( const QHash<QString, QVariant> &settings ) = 0;
        virtual QHash<QString, QVariant> settings() const = 0;
    };
    /**
     * @brief Method for getting a pointer to the configuration widget of the plugin.
     *
     * @return The configuration widget or, if no configuration widget exists, 0.
     */
    virtual ConfigWidget *configWidget();

    /** True if the plugin supports the given routing profile template */
    virtual bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

    /** Settings for the given routing profile template */
    virtual QHash<QString, QVariant> templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

protected:
    void setStatusMessage( const QString &message );

    void setSupportedCelestialBodies( const QStringList &celestialBodies );

    void setCanWorkOffline( bool canWorkOffline );

private:
    class Private;
    Private *const d;
};

}

Q_DECLARE_INTERFACE( Marble::RoutingRunnerPlugin, "org.kde.Marble.RunnerRunnerPlugin/1.00" )

#endif // MARBLE_ROUTINGRUNNERPLUGIN_H

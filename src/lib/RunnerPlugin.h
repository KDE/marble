//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_RUNNERPLUGIN_H
#define MARBLE_RUNNERPLUGIN_H

#include "PluginInterface.h"
#include "MarbleAbstractRunner.h"

#include <QtCore/QSettings>
#include <QtGui/QWidget>

#include "routing/RoutingProfilesModel.h"

namespace Marble
{

class RunnerPluginPrivate;

/**
  * A plugin for marble to execute a placemark search, reverse geocoding or routing task.
  */
class MARBLE_EXPORT RunnerPlugin : public QObject, public PluginInterface
{
    Q_OBJECT

public:
    enum Capability {
        None = 0x0, // The plugin is useless
        Search = 0x1, // The plugin can search for placemarks
        ReverseGeocoding = 0x2, // The plugin can do reverse geocoding
        Routing = 0x4, // The plugin can calculate routes
        Parsing = 0x8 // The plugin can parse files
    };

    /** One plugin can support multiple tasks */
    Q_DECLARE_FLAGS(Capabilities, Capability)

    /** Plugin factory method to create a new runner instance.
      * Method caller gets ownership of the returned object
      */
    virtual MarbleAbstractRunner* newRunner() const = 0;

    /** Constructor with optional parent object */
    explicit RunnerPlugin( QObject* parent = 0 );

    /** Destructor */
    virtual ~RunnerPlugin();

    /** Plugin capabilities, i.e. the tasks it supports */
    Capabilities capabilities() const;

    /** Convenience method to determine whether the plugin support the given capability */
    bool supports(Capability capability) const;

    /** True if the plugin supports its tasks on the given planet */
    bool supportsCelestialBody( const QString &celestialBodyId ) const;

    /** True if the plugin can execute its tasks without network access */
    bool canWorkOffline() const;

    /** Returns true if the plugin can execute a task for the given capability
      * The default implementation returns true iff the plugin supports
      * the given capability. This method can be overriden for example to
      * indicate an incomplete installation.
      */
    virtual bool canWork( Capability capability ) const;

    // Overridden methods with default implementations

    virtual QString name() const;

    virtual QString guiString() const;

    virtual QString nameId() const;

    virtual QString description() const;

    virtual QIcon icon() const;

    virtual void initialize();

    virtual bool isInitialized() const;

    /** A status message showing whether the plugin will be able to deliver results */
    QString statusMessage() const;

    class ConfigWidget : public QWidget
    {
    public:
        virtual void loadSettings( const QHash<QString, QVariant> &settings ) = 0;
        virtual QHash<QString, QVariant> settings() const = 0;
    };
    /**
     * Function for getting a pointer to the configuration widget of the plugin.
     *
     * @return: The configuration widget or, if no configuration widget exists, 0.
     */
    virtual ConfigWidget *configWidget();

    /** True if the plugin supports the given routing profile template */
    virtual bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

    /** Settings for the given routing profile template */
    virtual QHash<QString, QVariant> templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

protected:
    // Convenience methods for plugins to use
    void setCapabilities(Capabilities capabilities);

    void setSupportedCelestialBodies( const QStringList &celestialBodies );

    void setCanWorkOffline( bool canWorkOffline );

    void setName( const QString &name );

    void setGuiString( const QString &guiString );

    void setNameId( const QString &nameId );

    void setDescription( const QString &description );

    void setIcon( const QIcon &icon );

    void setStatusMessage( const QString &message );

private:
    RunnerPluginPrivate * const d;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Marble::RunnerPlugin::Capabilities)

Q_DECLARE_INTERFACE( Marble::RunnerPlugin, "org.kde.Marble.RunnerPlugin/1.02" )

#endif // MARBLE_RUNNERPLUGIN_H

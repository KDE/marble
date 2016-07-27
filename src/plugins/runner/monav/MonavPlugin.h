//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MONAVPLUGIN_H
#define MARBLE_MONAVPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class MonavMapsModel;
class MonavPluginPrivate;
class RouteRequest;

class MonavPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.MonavPlugin")
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    enum MonavRoutingDaemonVersion
    {
        Monav_0_2,
        Monav_0_3
    };

    explicit MonavPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    ~MonavPlugin();

    virtual RoutingRunner *newRunner() const;

    virtual bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const;

    virtual QHash< QString, QVariant > templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

    virtual ConfigWidget *configWidget();

    virtual bool canWork() const;

    QString mapDirectoryForRequest( const RouteRequest* request ) const;

    QStringList mapDirectoriesForRequest( const RouteRequest* request ) const;

    MonavMapsModel* installedMapsModel();

    void reloadMaps();

    MonavRoutingDaemonVersion monavVersion() const;

private:
    MonavPluginPrivate* const d;

    Q_PRIVATE_SLOT( d, void stopDaemon() )
};

}

#endif

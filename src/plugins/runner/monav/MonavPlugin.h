//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_MONAVPLUGIN_H
#define MARBLE_MONAVPLUGIN_H

#include "RunnerPlugin.h"

namespace Marble
{

class MonavMapsModel;
class MonavPluginPrivate;

class MonavPlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit MonavPlugin( QObject *parent = 0 );

    ~MonavPlugin();

    virtual MarbleAbstractRunner* newRunner() const;

    virtual bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const;

    virtual ConfigWidget *configWidget();

    virtual bool canWork( Capability capability ) const;

    QString mapDirectoryForRequest( RouteRequest* request ) const;

    MonavMapsModel* installedMapsModel();

    void reloadMaps();

private:
    MonavPluginPrivate* const d;
};

}

#endif

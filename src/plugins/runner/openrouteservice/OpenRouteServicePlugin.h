//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_OPENROUTESERVICEPLUGIN_H
#define MARBLE_OPENROUTESERVICEPLUGIN_H

#include "RunnerPlugin.h"

namespace Marble
{

class OpenRouteServicePlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit OpenRouteServicePlugin( QObject *parent = 0 );

    virtual MarbleAbstractRunner* newRunner() const;

    ConfigWidget* configWidget();

    virtual bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;
    virtual QHash<QString, QVariant> templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

};

}

#endif

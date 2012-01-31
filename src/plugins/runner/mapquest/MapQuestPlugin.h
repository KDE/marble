//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_MAPQUESTPLUGIN_H
#define MARBLE_MAPQUESTPLUGIN_H

#include "RunnerPlugin.h"

namespace Marble
{

class MapQuestPlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit MapQuestPlugin( QObject *parent = 0 );

    virtual MarbleAbstractRunner* newRunner() const;

    ConfigWidget* configWidget();

    virtual bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;
    virtual QHash<QString, QVariant> templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

};

}

#endif

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MarbleDeclarativePlugin.h"

#include "Coordinate.h"
#include "DeclarativeMapThemeManager.h"
#include "MarbleDeclarativeWidget.h"
#include "PositionSource.h"
#include "Tracking.h"
#include "Routing.h"
#include "Search.h"
#include "RouteRequestModel.h"
#include "ActivityModel.h"
#include "Activity.h"
#include "RelatedActivities.h"
#include "Settings.h"

#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeEngine>

namespace Marble
{
namespace Declarative
{

void MarbleDeclarativePlugin::registerTypes( const char * )
{
    const char* uri = "org.kde.edu.marble";

    qmlRegisterType<Marble::Declarative::Coordinate>( uri, 0, 11, "Coordinate" );
    qmlRegisterType<Marble::Declarative::PositionSource>( uri, 0, 11, "PositionSource" );
    qmlRegisterType<Marble::Declarative::Tracking>( uri, 0, 11, "Tracking" );
    qmlRegisterType<Marble::Declarative::Routing>( uri, 0, 11, "Routing" );
    qmlRegisterType<Marble::Declarative::Search>( uri, 0, 11, "Search" );
    qmlRegisterType<Marble::Declarative::RouteRequestModel>( uri, 0, 11, "RouteRequestModel" );
    qmlRegisterType<Marble::Declarative::ActivityModel>( uri, 0, 11, "ActivityModel" );
    qmlRegisterType<Marble::Declarative::Activity>( uri, 0, 11, "Activity" );
    qmlRegisterType<Marble::Declarative::RelatedActivities>( uri, 0, 11, "RelatedActivities" );
    qmlRegisterType<Marble::Declarative::Settings>( uri, 0, 11, "Settings" );

    qmlRegisterType<Marble::Declarative::MarbleWidget>( uri, 0, 11, "MarbleWidget" );
    qmlRegisterType<Marble::Declarative::MapThemeManager>( uri, 0, 11, "MapThemeManager" );
}

void MarbleDeclarativePlugin::initializeEngine( QDeclarativeEngine *engine, const char *)
{
    engine->addImageProvider( "maptheme", new MapThemeImageProvider );
}

}
}

#include "MarbleDeclarativePlugin.moc"

Q_EXPORT_PLUGIN2( MarbleDeclarativePlugin, Marble::Declarative::MarbleDeclarativePlugin )

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
#include "MarbleDeclarativeRunnerManager.h"
#include "PositionSource.h"
#include "Tracking.h"
#include "Routing.h"
#include "RouteRequestModel.h"

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
    qmlRegisterType<Marble::Declarative::RouteRequestModel>( uri, 0, 11, "RouteRequestModel" );

    qmlRegisterType<Marble::Declarative::MarbleWidget>( uri, 0, 11, "MarbleWidget" );
    qmlRegisterType<Marble::Declarative::MarbleRunnerManager>( uri, 0, 11, "MarbleRunnerManager" );
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

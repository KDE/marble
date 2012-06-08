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
#include "MarbleDeclarativeObject.h"
#include "MarbleDeclarativeWidget.h"
#include "PositionSource.h"
#include "Bookmarks.h"
#include "Tracking.h"
#include "Routing.h"
#include "Navigation.h"
#include "Search.h"
#include "RouteRequestModel.h"
#include "RelatedActivities.h"
#include "Settings.h"
#include "MapThemeModel.h"
#include "NewstuffModel.h"
#include "OfflineDataModel.h"
#include "Placemark.h"
#include "routing/SpeakersModel.h"
#include "routing/VoiceNavigationModel.h"
#include "AbstractFloatItem.h"
#include "RenderPlugin.h"
#include "DeclarativeDataPlugin.h"

#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeEngine>

void MarbleDeclarativePlugin::registerTypes( const char * )
{
    const char* uri = "org.kde.edu.marble";

    //@uri org.kde.edu.marble
    qmlRegisterType<Coordinate>( uri, 0, 11, "Coordinate" );
    qmlRegisterType<Placemark>( uri, 0, 11, "Placemark" );
    qmlRegisterType<PositionSource>( uri, 0, 11, "PositionSource" );
    qmlRegisterType<Bookmarks>( uri, 0, 11, "Bookmarks" );
    qmlRegisterType<Tracking>( uri, 0, 11, "Tracking" );
    qmlRegisterType<Routing>( uri, 0, 11, "Routing" );
    qmlRegisterType<Navigation>( uri, 0, 11, "Navigation" );
    qmlRegisterType<Search>( uri, 0, 11, "Search" );
    qmlRegisterType<RouteRequestModel>( uri, 0, 11, "RouteRequestModel" );
    qmlRegisterType<RelatedActivities>( uri, 0, 11, "RelatedActivities" );
    qmlRegisterType<Settings>( uri, 0, 11, "Settings" );

    qmlRegisterType<MarbleWidget>( uri, 0, 11, "MarbleWidget" );
    qmlRegisterType<MapThemeManager>( uri, 0, 11, "MapThemeManager" );
    qmlRegisterType<Marble::SpeakersModel>( uri, 0, 11, "SpeakersModel" );
    qmlRegisterType<Marble::VoiceNavigationModel>( uri, 0, 11, "VoiceNavigation" );
    qmlRegisterType<Marble::NewstuffModel>( uri, 0, 11, "NewstuffModel" );
    qmlRegisterType<OfflineDataModel>( uri, 0, 11, "OfflineDataModel" );
    qmlRegisterType<MapThemeModel>( uri, 0, 11, "MapThemeModel" );
    qmlRegisterType<DeclarativeDataPlugin>( uri, 0, 11, "DataLayer" );

    qmlRegisterUncreatableType<BookmarksModel>( uri, 0, 11, "BookmarksModel", "Do not create" );
    qmlRegisterUncreatableType<Marble::AbstractFloatItem>( uri, 0, 11, "FloatItem", "Do not create" );
    qmlRegisterUncreatableType<Marble::RenderPlugin>( uri, 0, 11, "RenderPlugin", "Do not create" );
}

void MarbleDeclarativePlugin::initializeEngine( QDeclarativeEngine *engine, const char *)
{
    engine->addImageProvider( "maptheme", new MapThemeImageProvider );
    // Register the global Marble object. Can be used in .qml files for requests like Marble.resolvePath("some/icon.png")
    if ( !engine->rootContext()->contextProperty( "Marble").isValid() ) {
        engine->rootContext()->setContextProperty( "Marble", new MarbleDeclarativeObject( this ) );
    }
}

#include "MarbleDeclarativePlugin.moc"

Q_EXPORT_PLUGIN2( MarbleDeclarativePlugin, MarbleDeclarativePlugin )

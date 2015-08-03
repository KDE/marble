//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "MarbleDeclarativePlugin.h"

#include "Coordinate.h"
#include "DeclarativeMapThemeManager.h"
#include "MarbleDeclarativeObject.h"
#include "MarbleDeclarativeWidget.h"
#include "PositionSource.h"
#include "Bookmarks.h"
#include "CloudSync.h"
#include "cloudsync/MergeItem.h"
#include "Tracking.h"
#include "Routing.h"
#include "Navigation.h"
#include "Search.h"
#include "RouteRequestModel.h"
#include "Settings.h"
#include "MapThemeModel.h"
#include "NewstuffModel.h"
#include "OfflineDataModel.h"
#include "Placemark.h"
#include "routing/SpeakersModel.h"
#include "routing/VoiceNavigationModel.h"
#include "AbstractFloatItem.h"
#include "RenderPlugin.h"
#include "MarblePlacemarkModel.h"
#include "DeclarativeDataPlugin.h"
#include "SearchBackend.h"
#include "MarbleQuickItem.h"

#include <QtQml/qqml.h>
#include <QQmlEngine>
#include <QQmlContext>

void MarbleDeclarativePlugin::registerTypes( const char *uri )
{
    qRegisterMetaType<Marble::MarbleMap*>("MarbleMap*");

    //@uri org.kde.edu.marble
    qmlRegisterType<Coordinate>( uri, 0, 20, "Coordinate" );
    qmlRegisterType<Placemark>( uri, 0, 20, "Placemark" );
    qmlRegisterType<PositionSource>( uri, 0, 20, "PositionSource" );
    qmlRegisterType<Bookmarks>( uri, 0, 20, "Bookmarks" );
    qmlRegisterType<Tracking>( uri, 0, 20, "Tracking" );
    qmlRegisterType<Marble::Routing>( uri, 0, 20, "Routing" );
    qmlRegisterType<Navigation>( uri, 0, 20, "Navigation" );
    qmlRegisterType<Search>( uri, 0, 20, "Find" );
    qmlRegisterType<CloudSync>( uri, 0, 20, "CloudSync" );
    qmlRegisterType<Marble::MergeItem>( uri, 0, 20, "MergeItem" );
    qmlRegisterType<RouteRequestModel>( uri, 0, 20, "RouteRequestModel" );
    qmlRegisterType<Settings>( uri, 0, 20, "Settings" );

    qmlRegisterType<MarbleWidget>( uri, 0, 20, "MarbleWidget" );
    qmlRegisterType<MapThemeManager>( uri, 0, 20, "MapThemeManager" );
    qmlRegisterType<Marble::SpeakersModel>( uri, 0, 20, "SpeakersModel" );
    qmlRegisterType<Marble::VoiceNavigationModel>( uri, 0, 20, "VoiceNavigation" );
    qmlRegisterType<Marble::NewstuffModel>( uri, 0, 20, "NewstuffModel" );
    qmlRegisterType<OfflineDataModel>( uri, 0, 20, "OfflineDataModel" );
    qmlRegisterType<MapThemeModel>( uri, 0, 20, "MapThemeModel" );
    qmlRegisterType<DeclarativeDataPlugin>( uri, 0, 20, "DataLayer" );

    qmlRegisterType<Marble::SearchBackend>(uri, 0, 20, "SearchBackend");
    qmlRegisterType<Marble::MarbleQuickItem>(uri, 0, 20, "MarbleItem");

    qmlRegisterUncreatableType<Marble::MarblePlacemarkModel>(uri, 1, 0, "MarblePlacemarkModel", "MarblePlacemarkModel is not instantiable");
    qmlRegisterUncreatableType<BookmarksModel>( uri, 0, 20, "BookmarksModel", "Do not create" );
    qmlRegisterUncreatableType<Marble::AbstractFloatItem>( uri, 0, 20, "FloatItem", "Do not create" );
    qmlRegisterUncreatableType<Marble::RenderPlugin>( uri, 0, 20, "RenderPlugin", "Do not create" );
}

void MarbleDeclarativePlugin::initializeEngine( QQmlEngine *engine, const char *)
{
    engine->addImageProvider( "maptheme", new MapThemeImageProvider );
    // Register the global Marble object. Can be used in .qml files for requests like Marble.resolvePath("some/icon.png")
    if ( !engine->rootContext()->contextProperty( "Marble").isValid() ) {
        engine->rootContext()->setContextProperty( "Marble", new MarbleDeclarativeObject( this ) );
    }
}

#include "moc_MarbleDeclarativePlugin.cpp"

Q_EXPORT_PLUGIN2( MarbleDeclarativePlugin, MarbleDeclarativePlugin )

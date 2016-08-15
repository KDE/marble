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
#include "PositionSource.h"
#include "Bookmarks.h"
#include "Tracking.h"
#include "Routing.h"
#include "Navigation.h"
#include "RouteRequestModel.h"
#include "Settings.h"
#include "MapThemeModel.h"
#include "NewstuffModel.h"
#include "OfflineDataModel.h"
#include "Placemark.h"
#include "routing/SpeakersModel.h"
#include "routing/VoiceNavigationModel.h"
#include "routing/RoutingModel.h"
#include "AbstractFloatItem.h"
#include "RenderPlugin.h"
#include "MarblePlacemarkModel.h"
#include "DeclarativeDataPlugin.h"
#include "SearchBackend.h"
#include "MarbleQuickItem.h"

#include <QQmlEngine>
#include <QQmlContext>

void MarbleDeclarativePlugin::registerTypes( const char *uri )
{
    qRegisterMetaType<Marble::MarbleMap*>("MarbleMap*");

    //@uri org.kde.marble
    qmlRegisterType<Coordinate>( uri, 0, 20, "Coordinate" );
    qmlRegisterType<Marble::Placemark>( uri, 0, 20, "Placemark" );
    qmlRegisterType<Marble::PositionSource>( uri, 0, 20, "PositionSource" );
    qmlRegisterType<Marble::Bookmarks>( uri, 0, 20, "Bookmarks" );
    qmlRegisterType<Marble::Tracking>( uri, 0, 20, "Tracking" );
    qmlRegisterType<Marble::Routing>( uri, 0, 20, "Routing" );
    qmlRegisterType<Marble::Navigation>( uri, 0, 20, "Navigation" );
    qmlRegisterType<RouteRequestModel>( uri, 0, 20, "RouteRequestModel" );
    qmlRegisterType<Settings>( uri, 0, 20, "Settings" );

    qmlRegisterType<MapThemeManager>( uri, 0, 20, "MapThemeManager" );
    qmlRegisterType<Marble::SpeakersModel>( uri, 0, 20, "SpeakersModel" );
    qmlRegisterType<Marble::VoiceNavigationModel>( uri, 0, 20, "VoiceNavigation" );
    qmlRegisterType<Marble::NewstuffModel>( uri, 0, 20, "NewstuffModel" );
    qmlRegisterType<OfflineDataModel>( uri, 0, 20, "OfflineDataModel" );
    qmlRegisterType<MapThemeModel>( uri, 0, 20, "MapThemeModel" );
    qmlRegisterType<DeclarativeDataPlugin>( uri, 0, 20, "DataLayer" );

    qmlRegisterType<Marble::SearchBackend>(uri, 0, 20, "SearchBackend");
    qRegisterMetaType<Marble::MarblePlacemarkModel*>("MarblePlacemarkModel*");
    qmlRegisterType<Marble::MarbleQuickItem>(uri, 0, 20, "MarbleItem");

    qmlRegisterUncreatableType<Marble::MarblePlacemarkModel>(uri, 1, 0, "MarblePlacemarkModel",
                                                             QStringLiteral("MarblePlacemarkModel is not instantiable"));
    qmlRegisterUncreatableType<Marble::RoutingModel>(uri, 0, 20, "RoutingModel",
                                                     QStringLiteral("RoutingModel is not instantiable"));
    qmlRegisterUncreatableType<Marble::BookmarksModel>(uri, 0, 20, "BookmarksModel",
                                                       QStringLiteral("Do not create"));
    qmlRegisterUncreatableType<Marble::AbstractFloatItem>(uri, 0, 20, "FloatItem",
                                                          QStringLiteral("Do not create"));
    qmlRegisterUncreatableType<Marble::RenderPlugin>(uri, 0, 20, "RenderPlugin",
                                                     QStringLiteral("Do not create"));
    qmlRegisterUncreatableType<Marble::MarbleMap>(uri, 0, 20, "MarbleMap",
                                                  QStringLiteral("Do not create"));
}

void MarbleDeclarativePlugin::initializeEngine( QQmlEngine *engine, const char *)
{
    engine->addImageProvider(QStringLiteral("maptheme"), new MapThemeImageProvider );
    // Register the global Marble object. Can be used in .qml files for requests like Marble.resolvePath("some/icon.png")
    const QString marbleObjectName = QStringLiteral("Marble");
    if (!engine->rootContext()->contextProperty(marbleObjectName).isValid()) {
        engine->rootContext()->setContextProperty(marbleObjectName, new MarbleDeclarativeObject(this));
    }
}

#include "moc_MarbleDeclarativePlugin.cpp"

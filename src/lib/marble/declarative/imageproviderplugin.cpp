// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later
//
#include "imageproviderplugin.h"
#include "DeclarativeMapThemeManager.h"
#include "MapThemeModel.h"

void ImageProviderPlugin::registerTypes(const char *uri)
{
    qmlRegisterTypesAndRevisions<MapThemeModel>(uri, 254);
}

void ImageProviderPlugin::initializeEngine(QQmlEngine *engine, const char *)
{
    engine->addImageProvider(QStringLiteral("maptheme"), new MapThemeImageProvider);
}

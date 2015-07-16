//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#include "GeoNamesWeatherItem.h"

using namespace Marble;

GeoNamesWeatherItem::GeoNamesWeatherItem( QObject *parent )
    : WeatherItem( parent )
{
}

GeoNamesWeatherItem::~GeoNamesWeatherItem()
{
}

void GeoNamesWeatherItem::addDownloadedFile( const QString& url, const QString& type )
{
    Q_UNUSED( url )
    Q_UNUSED( type )
}

QString GeoNamesWeatherItem::service() const
{
    return QString( "GeoNames" );
}

QString GeoNamesWeatherItem::creditHtml() const
{
    return tr( "Supported by <a href=\"http://www.geonames.org/export/JSON-webservices.html\" target=\"_BLANK\">geonames.org</a>" );
}

#include "moc_GeoNamesWeatherItem.cpp"

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "FakeWeatherItem.h"

using namespace Marble;

FakeWeatherItem::FakeWeatherItem( QObject *parent )
    : WeatherItem( parent )
{
}

FakeWeatherItem::~FakeWeatherItem()
{
}

QString FakeWeatherItem::service() const
{
    return QString( "fake" );
}

void FakeWeatherItem::addDownloadedFile( const QString& url, const QString& type )
{
    // There are no downloadable files for the fake backend
    Q_UNUSED( url );
    Q_UNUSED( type );
}

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#ifndef GEONAMESWEATHERITEM_H
#define GEONAMESWEATHERITEM_H

#include "WeatherItem.h"

namespace Marble
{

class GeoNamesWeatherItem : public WeatherItem
{
    Q_OBJECT

 public:
    explicit GeoNamesWeatherItem( QObject *parent = 0 );
    ~GeoNamesWeatherItem();

    void addDownloadedFile( const QString& url, const QString& type );

    QString service() const;
    QString creditHtml() const;
};

} // namespace Marble

#endif // GEONAMESWEATHERITEM_H

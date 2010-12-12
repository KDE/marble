//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCWEATHERITEM_H
#define BBCWEATHERITEM_H

#include "WeatherItem.h"

class QString;
class QUrl;

namespace Marble
{

class BBCWeatherItem : public WeatherItem
{
    Q_OBJECT

 public:
    BBCWeatherItem( QObject *parent = 0 );
    ~BBCWeatherItem();
    
    virtual bool request( const QString& type );

    QString service() const;
    void addDownloadedFile( const QString& url, const QString& type );
    
    QUrl observationUrl() const;
    QUrl forecastUrl() const;
   
    quint32 bbcId() const;
    void setBbcId( quint32 id );

    QString creditHtml() const;
 private:    
    quint32 m_bbcId;
    bool m_observationRequested;
    bool m_forecastRequested;
};

} // namespace Marble

#endif // BBCWEATHERITEM_H

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef ABSTRACTWEATHERSERVICE_H
#define ABSTRACTWEATHERSERVICE_H

// Marble
#include "WeatherModel.h"

namespace Marble
{

class MarbleWidget;

class AbstractWeatherService : public QObject
{
    Q_OBJECT
    
 public:
    explicit AbstractWeatherService( QObject *parent );
    virtual ~AbstractWeatherService();
    void setMarbleWidget( MarbleWidget* widget );
    
 public Q_SLOTS:
    virtual void setFavoriteItems( const QStringList& favorite );
    QStringList favoriteItems() const;

    virtual void getAdditionalItems( const GeoDataLatLonAltBox& box,
                                     const MarbleModel *model,
                                     qint32 number = 10 ) = 0;
    virtual void getItem( const QString &id, const MarbleModel *model ) = 0;
    virtual void parseFile( const QByteArray& file );
    
 Q_SIGNALS:
    void requestedDownload( const QUrl& url, const QString& type, AbstractDataPluginItem *item );
    void createdItems( QList<AbstractDataPluginItem*> items );
    void downloadDescriptionFileRequested( const QUrl& );

protected:
    MarbleWidget* marbleWidget();

private:
    QStringList m_favoriteItems;
    MarbleWidget* m_marbleWidget;
};

} // namespace Marble

#endif // ABSTRACTWEATHERSERVICE_H

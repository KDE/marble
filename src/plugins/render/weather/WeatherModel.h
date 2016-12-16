//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef WEATHERMODEL_H
#define WEATHERMODEL_H

#include "AbstractDataPluginModel.h"

#include "WeatherData.h"

#include <QStringList>

class QByteArray;
class QTimer;

namespace Marble
{

class AbstractWeatherService;
class GeoDataLatLonAltBox;
class MarbleWidget;
class MarbleModel;
    
class WeatherModel : public AbstractDataPluginModel
{
    Q_OBJECT
    
 public:
    explicit WeatherModel( const MarbleModel *marbleModel, QObject *parent );
    ~WeatherModel() override;

    void setUpdateInterval( quint32 hours );

    void setFavoriteItems( const QStringList& list ) override;
    
 public Q_SLOTS:
    /**
     * Downloads the file from @p url. @p item -> addDownloadedFile() will be called when the
     * download is finished. Additionally initialized() items will be added to the item list
     * after the download.
     * @param: The type of the download
     **/
    void downloadItemData( const QUrl& url, const QString& type, AbstractDataPluginItem *item );
    
    void downloadDescriptionFileRequested( const QUrl& url );

    void setMarbleWidget(MarbleWidget *widget);

 Q_SIGNALS:
    void favoriteItemChanged( const QString& id, bool isFavorite );

 protected:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             qint32 number = 10 ) override;
    void getItem( const QString &id ) override;
    void parseFile( const QByteArray& file ) override;

 private:
    void addService( AbstractWeatherService *service );

    QList<AbstractWeatherService*> m_services;
    QTimer *m_timer;
};

} // namespace Marble

#endif // WEATHERMODEL_H

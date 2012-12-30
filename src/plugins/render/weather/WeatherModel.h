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
#include "GeoDataLatLonAltBox.h"
#include "MarbleWidget.h"

#include <QtCore/QStringList>

class QByteArray;
class QTimer;

namespace Marble
{

class AbstractWeatherService;
class GeoDataLatLonAltBox;
class MarbleModel;
    
class WeatherModel : public AbstractDataPluginModel
{
    Q_OBJECT
    
 public:
    explicit WeatherModel( const PluginManager *pluginManager, QObject *parent );
    ~WeatherModel();

    void setUpdateInterval( quint32 hours );

    void setFavoriteItems( const QStringList& list );
    
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
    void additionalItemsRequested( const GeoDataLatLonAltBox &,
                                   const MarbleModel *,
                                   qint32 number );
    void favoriteItemChanged( const QString& id, bool isFavorite );
    void parseFileRequested( const QByteArray& file );

 private Q_SLOTS:
    void updateItems();

 protected:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             const MarbleModel *marbleModel,
                             qint32 number = 10 );
    virtual void getItem( const QString &id, const MarbleModel *model );
    void parseFile( const QByteArray& file );

 private:
    void createServices();
    void addService( AbstractWeatherService *service );

    bool m_initialized;
    QList<AbstractWeatherService*> m_services;
    QTimer *m_timer;
    GeoDataLatLonAltBox m_lastBox;
    const MarbleModel *m_lastModel;
    qint32 m_lastNumber;
};

} // namespace Marble

#endif // WEATHERMODEL_H

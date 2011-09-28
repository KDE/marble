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

class QByteArray;

namespace Marble
{

class AbstractWeatherService;
class GeoDataLatLonAltBox;
class MarbleModel;
    
class WeatherModel : public AbstractDataPluginModel
{
    Q_OBJECT
    
 public:
    explicit WeatherModel( const PluginManager *pluginManager,
                           QObject *parent );
    ~WeatherModel();
    
 public Q_SLOTS:
    /**
     * Downloads the file from @p url. @p item -> addDownloadedFile() will be called when the
     * download is finished. Additionally initialized() items will be added to the item list
     * after the download.
     * @param: The type of the download
     **/
    void downloadItemData( const QUrl& url, const QString& type, AbstractDataPluginItem *item );
    
    /**
     * Adds the @p item to the list of initialized items.
     */
    void addItemToList( AbstractDataPluginItem *item );
    
 Q_SIGNALS:
    void additionalItemsRequested( const GeoDataLatLonAltBox &,
                                   const MarbleModel *,
                                   qint32 number );
    
 protected:
    void getAdditionalItems( const GeoDataLatLonAltBox& box,
                             const MarbleModel *marbleModel,
                                   qint32 number = 10 );
    void parseFile( const QByteArray& file );
 
 private:
    void addService( AbstractWeatherService *service );
};

} // namespace Marble

#endif // WEATHERMODEL_H

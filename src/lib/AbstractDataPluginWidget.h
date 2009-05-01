//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef ABSTRACTDATAPLUGINWIDGET_H
#define ABSTRACTDATAPLUGINWIDGET_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "marble_export.h"

namespace Marble {
    
class AbstractDataPluginWidgetPrivate;
class GeoDataCoordinates;
class GeoPainter;
class GeoSceneLayer;
class CacheStoragePolicy;
class ViewportParams;

class MARBLE_EXPORT AbstractDataPluginWidget : public QObject {
    Q_OBJECT
    
 public:
    AbstractDataPluginWidget( QObject *parent = 0 );
    ~AbstractDataPluginWidget();
    
    GeoDataCoordinates coordinates();
    void setCoordinates( GeoDataCoordinates coordinates );
     
    QString id();
    void setId( QString id );
    
    /**
      * Returning the angular resolution of the viewport when the widget was added to it the last
      * time.
      */
    qreal addedAngularResolution();
    void setAddedAngularResolution( qreal resolution );
    
    virtual QString widgetType() const = 0;
     
    virtual bool initialized() = 0;
    
    virtual void addDownloadedFile( QString url, QString type ) = 0;
    
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
                         
    virtual bool operator<( const AbstractDataPluginWidget *other ) const = 0;
 private:
    AbstractDataPluginWidgetPrivate * const d;
};

} // Marble namespace

#endif // ABSTRACTDATAPLUGINWIDGET_H

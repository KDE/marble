//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef ABSTRACTDATAPLUGINITEM_H
#define ABSTRACTDATAPLUGINITEM_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "GeoGraphicsItem.h"
#include "marble_export.h"

class QAction;
class QRect;
class QSize;
class QPoint;

namespace Marble {
    
class AbstractDataPluginItemPrivate;
class GeoDataCoordinates;
class GeoPainter;
class GeoSceneLayer;
class CacheStoragePolicy;
class ViewportParams;

class MARBLE_EXPORT AbstractDataPluginItem : public QObject, public GeoGraphicsItem {
    Q_OBJECT
    
 public:
    AbstractDataPluginItem( QObject *parent = 0 );
    virtual ~AbstractDataPluginItem();
    
    QString target();
    void setTarget( const QString& target );
     
    QString id() const;
    void setId( const QString& id );
    
    /**
      * Returning the angular resolution of the viewport when the item was added to it the last
      * time.
      */
    qreal addedAngularResolution() const;
    void setAddedAngularResolution( qreal resolution );
    
    virtual QAction *action();
    
    virtual QString itemType() const = 0;
     
    virtual bool initialized() = 0;
    
    virtual void addDownloadedFile( const QString& url, const QString& type );
    
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
                         
    virtual bool operator<( const AbstractDataPluginItem *other ) const = 0;
    
 private:
    AbstractDataPluginItemPrivate * const d;
};

} // Marble namespace

#endif // ABSTRACTDATAPLUGINITEM_H

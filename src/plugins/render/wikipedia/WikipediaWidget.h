//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef WIKIPEDIAWIDGET_H
#define WIKIPEDIAWIDGET_H

#include "AbstractDataPluginWidget.h"

#include <QtCore/QUrl>

class QPixmap;
class QWebView;

namespace Marble {
 
class WikipediaWidget : public AbstractDataPluginWidget {
 public:
    WikipediaWidget( QObject *parent );
    
    ~WikipediaWidget();
    
    QString widgetType() const;
     
    bool initialized();
    
    void addDownloadedFile( QString url, QString type );
    
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos, GeoSceneLayer * layer = 0 );
                 
    bool operator<( const AbstractDataPluginWidget *other ) const;
    
    qreal longitude();
    
    void setLongitude( qreal longitude );
    
    qreal latitude();
    
    void setLatitude( qreal latitude );
    
    QUrl url();
    
    void setUrl( QUrl url );
    
    QUrl thumbnailImageUrl();
    
    void setThumbnailImageUrl( QUrl thumbnailImageUrl );
    
    bool eventFilter( QObject *, QMouseEvent * );
    
    void setIcon( QPixmap *icon );
 private:
    QUrl m_url;
    QUrl m_thumbnailImageUrl;
    QWebView *m_browser;
    
    QPixmap *m_icon;
};
    
}

#endif // WIKIPEDIAWIDGET_H

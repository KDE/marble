//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef PHOTOPLUGINWIDGET_H
#define PHOTOPLUGINWIDGET_H

#include "AbstractDataPluginWidget.h"

#include <QtGui/QPixmap>

class QUrl;
class QWebView;

namespace Marble {
 
class PhotoPluginWidget : public AbstractDataPluginWidget {
    Q_OBJECT
 public:
    explicit PhotoPluginWidget( QObject *parent );
    ~PhotoPluginWidget();
    
    QString widgetType() const;
    
    bool initialized();
    
    void addDownloadedFile( QString url, QString type );
    
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos, GeoSceneLayer * layer = 0 );
                 
    bool operator<( const AbstractDataPluginWidget *other ) const;
    
    QUrl photoUrl() const;
    
    QUrl infoUrl( QString apiKey ) const;
    
    QString server() const;
    
    void setServer( QString server );
    
    QString farm() const;
    
    void setFarm( QString farm );
    
    QString secret() const;
    
    void setSecret( QString secret );
    
    QString owner() const;
    
    void setOwner( QString owner );
    
    bool eventFilter( QObject *, QMouseEvent * );
 private:
    bool m_hasCoordinates;
    QPixmap m_smallImage;
    QWebView *m_browser;
    
    QString m_server;
    QString m_farm;
    QString m_secret;
    QString m_owner;
};
    
}

#endif //PHOTOPLUGINWIDGET_H

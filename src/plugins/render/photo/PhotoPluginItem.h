//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef PHOTOPLUGINITEM_H
#define PHOTOPLUGINITEM_H

#include "AbstractDataPluginItem.h"

#include <QtGui/QPixmap>

class QAction;
class QUrl;
class QWebView;

namespace Marble {
 
class PhotoPluginItem : public AbstractDataPluginItem {
    Q_OBJECT
 public:
    explicit PhotoPluginItem( QObject *parent );
    ~PhotoPluginItem();
    
    QString name() const;
    
    QString itemType() const;
    
    bool initialized();
    
    void addDownloadedFile( const QString& url, const QString& type );
    
    void paint( GeoPainter *painter, ViewportParams *viewport,
                const QString& renderPos, GeoSceneLayer * layer = 0 );
                 
    bool operator<( const AbstractDataPluginItem *other ) const;
    
    QUrl photoUrl() const;
    
    QUrl infoUrl() const;
    
    QString server() const;
    
    void setServer( const QString& server );
    
    QString farm() const;
    
    void setFarm( const QString& farm );
    
    QString secret() const;
    
    void setSecret( const QString& secret );
    
    QString owner() const;
    
    void setOwner( const QString& owner );
    
    QString title() const;
    
    void setTitle( const QString& title );
    
    QAction *action();
    
 public Q_SLOTS:
    void openBrowser();
    
 private:
    bool m_hasCoordinates;
    QPixmap m_smallImage;
    QPixmap m_microImage;
    QWebView *m_browser;
    QAction *m_action;
    
    QString m_server;
    QString m_farm;
    QString m_secret;
    QString m_owner;
    QString m_title;
};
    
}

#endif //PHOTOPLUGINITEM_H

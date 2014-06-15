//
// This file is part of the Marble Virtual Globe.
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

#include "LabelGraphicsItem.h"

#include <QImage>

class QAction;
class QUrl;

namespace Marble
{

class MarbleWidget;
class TinyWebBrowser;
 
class PhotoPluginItem : public AbstractDataPluginItem
{
    Q_OBJECT
 public:
    explicit PhotoPluginItem( MarbleWidget *widget, QObject *parent );
    ~PhotoPluginItem();
    
    QString name() const;
    
    bool initialized() const;
    
    void addDownloadedFile( const QString& url, const QString& type );
                 
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
    MarbleWidget *m_marbleWidget;

    LabelGraphicsItem m_image;
    QImage m_smallImage;
    QImage m_microImage;
    TinyWebBrowser *m_browser;
    QAction *m_action;
    
    QString m_server;
    QString m_farm;
    QString m_secret;
    QString m_owner;
    QString m_title;
};
    
}

#endif //PHOTOPLUGINITEM_H

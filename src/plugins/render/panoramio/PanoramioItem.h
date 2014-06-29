//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef PANORAMIOITEM_H
#define PANORAMIOITEM_H

#include "AbstractDataPluginItem.h"

#include <QDate>
#include <QImage>
#include <QUrl>

class QAction;

namespace Marble
{

class MarbleWidget;

const QString standardImageSize = "mini_square";

class PanoramioItem : public AbstractDataPluginItem
{
    Q_OBJECT

 public:
    explicit PanoramioItem( MarbleWidget *marbleWidget, QObject *parent );

    bool initialized() const;

    void addDownloadedFile( const QString &url, const QString &type );

    void setPhotoUrl( const QUrl &url );

    QDate uploadDate() const;

    void setUploadDate( const QDate &uploadDate );

    void paint( QPainter *painter );

    QAction *action();

    bool operator<( const AbstractDataPluginItem *other ) const;

 public Q_SLOTS:
    void openBrowser();

 private:
    MarbleWidget *const m_marbleWidget;
    QAction *m_action;
    QImage smallImage;
    QUrl m_url;
    QDate m_uploadDate;
};

}

#endif // PANORAMIOITEM_H

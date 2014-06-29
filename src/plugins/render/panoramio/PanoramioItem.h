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

namespace Marble
{

const QString standardImageSize = "mini_square";

class PanoramioItem : public AbstractDataPluginItem
{
    Q_OBJECT

 public:
    explicit PanoramioItem( QObject *parent );

    bool initialized() const;

    void addDownloadedFile( const QString &url, const QString &type );

    QDate uploadDate() const;

    void setUploadDate( const QDate &uploadDate );

    void paint( QPainter *painter );

    bool operator<( const AbstractDataPluginItem *other ) const;

 private:
    QImage smallImage;
    QDate m_uploadDate;
};

}

#endif // PANORAMIOITEM_H

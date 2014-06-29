//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef PANORAMIOWIDGET_H
#define PANORAMIOWIDGET_H

#include "AbstractDataPluginItem.h"

#include <QDate>
#include <QPixmap>

namespace Marble
{

const QString standardImageSize = "mini_square";

class PanoramioWidget : public AbstractDataPluginItem
{
    Q_OBJECT

 public:
    explicit PanoramioWidget( QObject *parent );
    
    bool initialized() const;
    
    void addDownloadedFile( const QString &url, const QString &type );
    
    QDate uploadDate() const;
    
    void setUploadDate( QDate uploadDate );
    
    void paint( QPainter *painter );
                 
    bool operator<( const AbstractDataPluginItem *other ) const;
 private:
    QImage smallImage;
    QDate m_uploadDate;
};

}

#endif // PANORAMIOWIDGET_H

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

#include "AbstractDataPluginWidget.h"

#include <QtCore/QDate>
#include <QtGui/QPixmap>

namespace Marble
{
    
const QString standardImageSize = "mini_square";
 
class PanoramioWidget : public AbstractDataPluginWidget
{
 public:
    PanoramioWidget( QObject *parent );
    
    QString widgetType() const;
     
    bool initialized();
    
    void addDownloadedFile( QString url, QString type );
    
    QDate uploadDate() const;
    
    void setUploadDate( QDate uploadDate );
    
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos, GeoSceneLayer * layer = 0 );
                 
    bool operator<( const AbstractDataPluginWidget *other ) const;
 private:
    QImage smallImage;
    QDate m_uploadDate;
};
    
}

#endif // PANORAMIOWIDGET_H

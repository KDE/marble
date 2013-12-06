//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#ifndef MARBLE_TOURWIDGET_H
#define MARBLE_TOURWIDGET_H

#include "marble_export.h"

#include <QWidget>

class QModelIndex;

namespace Marble
{

class FileManager;
class GeoDataPlacemark;
class GeoDataLatLonBox;
class GeoDataTreeModel;
class MarbleWidget;

class TourWidgetPrivate;

class MARBLE_EXPORT TourWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit TourWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~TourWidget();

     void setMarbleWidget( MarbleWidget *widget );

 private:
    Q_PRIVATE_SLOT( d, void openFile() )
    Q_PRIVATE_SLOT( d, void mapCenterOn( const QModelIndex &index ) )
    Q_DISABLE_COPY( TourWidget )

    TourWidgetPrivate * const d;
};

}

#endif

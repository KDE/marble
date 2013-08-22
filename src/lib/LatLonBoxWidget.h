//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007 Torsten Rahn <tackat@kde.org>

#ifndef MARBLE_LATLONBOXWIDGET_H
#define MARBLE_LATLONBOXWIDGET_H

#include <QWidget>

#include "marble_export.h"

namespace Marble
{
class GeoDataLatLonBox;

class MARBLE_EXPORT LatLonBoxWidget: public QWidget
{
    Q_OBJECT

 public:
    explicit LatLonBoxWidget( QWidget * const parent = 0, Qt::WindowFlags const f = 0 );
    ~LatLonBoxWidget();
    GeoDataLatLonBox latLonBox() const;
    void setLatLonBox( GeoDataLatLonBox const & );

 Q_SIGNALS:
    void valueChanged();

 private Q_SLOTS:
    void updateLatSingleStep();
    void updateLonSingleStep();

 private:
    Q_DISABLE_COPY( LatLonBoxWidget )
    class Private;
    Private * const d;
};

}

#endif

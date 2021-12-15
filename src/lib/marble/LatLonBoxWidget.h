// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <tackat@kde.org>

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
    explicit LatLonBoxWidget( QWidget * const parent = nullptr, Qt::WindowFlags const f = Qt::WindowFlags() );
    ~LatLonBoxWidget() override;
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

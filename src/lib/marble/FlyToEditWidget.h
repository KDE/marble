//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// Copyright 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef FLYTOEDITWIDGET_H
#define FLYTOEDITWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include "GeoDataCoordinates.h"

class QDoubleSpinBox;

namespace Marble
{

class GeoDataFlyTo;
class MarbleWidget;

class FlyToEditWidget: public QWidget
{
    Q_OBJECT

public:
    FlyToEditWidget( const QModelIndex& index, MarbleWidget* widget, QWidget* parent=0 );

Q_SIGNALS:
    void editingDone( const QModelIndex& index );

private Q_SLOTS:
    void save();
    void updateCoordinates();

private:
    GeoDataFlyTo* flyToElement();
    MarbleWidget* m_widget;
    QModelIndex m_index;
    QDoubleSpinBox *m_waitSpin;
    GeoDataCoordinates m_coord;
};

} // namespace Marble

#endif

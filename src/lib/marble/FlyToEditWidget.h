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
#include <QPersistentModelIndex>

#include "GeoDataCoordinates.h"

class QDoubleSpinBox;
class QToolButton;
class QComboBox;

namespace Marble
{

class GeoDataFlyTo;
class MarbleWidget;

class FlyToEditWidget: public QWidget
{
    Q_OBJECT

public:
    FlyToEditWidget( const QModelIndex& index, MarbleWidget* widget, QWidget* parent=0 );
    bool editable() const;

Q_SIGNALS:
    void editingDone( const QModelIndex& index );

public Q_SLOTS:
    void setEditable( bool editable );

    /** We disable editing of wait duration for first flyto in playlist. */
    void setFirstFlyTo(const QPersistentModelIndex &index );

private Q_SLOTS:
    void save();
    void updateCoordinates();

private:
    GeoDataFlyTo* flyToElement();
    MarbleWidget* m_widget;
    QPersistentModelIndex m_index;
    QDoubleSpinBox *m_durationSpin;
    QComboBox *m_modeCombo;
    GeoDataCoordinates m_coord;
    QToolButton *m_button;
};

} // namespace Marble

#endif

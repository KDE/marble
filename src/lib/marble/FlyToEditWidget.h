// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
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
    FlyToEditWidget( const QModelIndex& index, MarbleWidget* widget, QWidget* parent=nullptr );
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

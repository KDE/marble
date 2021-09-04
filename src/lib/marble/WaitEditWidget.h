// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef WAITEDITWIDGET_H
#define WAITEDITWIDGET_H

#include <QWidget>
#include <QModelIndex>

class QDoubleSpinBox;
class QToolButton;

namespace Marble
{

class GeoDataWait;

class WaitEditWidget: public QWidget
{
    Q_OBJECT

public:
    explicit WaitEditWidget( const QModelIndex& index, QWidget* parent=nullptr );
    bool editable() const;

Q_SIGNALS:
    void editingDone( const QModelIndex& index );

public Q_SLOTS:
    void setEditable( bool editable );

private Q_SLOTS:
    void save();

private:
    GeoDataWait* waitElement();
    QPersistentModelIndex m_index;
    QDoubleSpinBox *m_spinBox;
    QToolButton *m_button;
};

} // namespace Marble

#endif

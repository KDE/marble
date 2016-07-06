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

#ifndef TOURCONTROLEDITWIDGET_H
#define TOURCONTROLEDITWIDGET_H

#include <QWidget>
#include <QPersistentModelIndex>

class QRadioButton;
class QToolButton;

namespace Marble
{

class GeoDataTourControl;

class TourControlEditWidget: public QWidget
{
    Q_OBJECT

public:
    explicit TourControlEditWidget( const QModelIndex& index, QWidget* parent=0 );
    bool editable() const;

Q_SIGNALS:
    void editingDone( const QModelIndex& index );

public Q_SLOTS:
    void setEditable( bool editable );

private Q_SLOTS:
    void save();

private:
    GeoDataTourControl* tourControlElement();
    QPersistentModelIndex m_index;
    QRadioButton *m_radio_play;
    QRadioButton *m_radio_pause;
    QToolButton *m_button;
};

} // namespace Marble

#endif

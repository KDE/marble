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

#ifndef SOUNDCUEEDITWIDGET_H
#define SOUNDCUEEDITWIDGET_H

#include <QWidget>
#include <QPersistentModelIndex>

class QLineEdit;
class QToolButton;

namespace Marble
{

class GeoDataSoundCue;

class SoundCueEditWidget: public QWidget
{
    Q_OBJECT

public:
    explicit SoundCueEditWidget( const QModelIndex& index, QWidget* parent=0 );
    bool editable() const;

Q_SIGNALS:
    void editingDone( const QModelIndex& index );

public Q_SLOTS:
    void setEditable( bool editable );

private Q_SLOTS:
    void save();
    void open();

private:
    GeoDataSoundCue* soundCueElement();
    QPersistentModelIndex m_index;
    QLineEdit* m_lineEdit;
    QToolButton *m_button;
    QToolButton *m_button2;
};

} // namespace Marble

#endif

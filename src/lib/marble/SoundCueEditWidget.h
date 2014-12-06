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
#include <QModelIndex>

class QLineEdit;

namespace Marble
{

class GeoDataSoundCue;

class SoundCueEditWidget: public QWidget
{
    Q_OBJECT

public:
    SoundCueEditWidget( const QModelIndex& index, QWidget* parent=0 );

Q_SIGNALS:
    void editingDone( const QModelIndex& index );

private Q_SLOTS:
    void save();

private:
    GeoDataSoundCue* soundCueElement();
    QModelIndex m_index;
    QLineEdit* m_lineEdit;
};

} // namespace Marble

#endif

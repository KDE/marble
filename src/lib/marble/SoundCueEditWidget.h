// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
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
    explicit SoundCueEditWidget( const QModelIndex& index, QWidget* parent=nullptr );
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

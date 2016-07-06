//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Mihail Ivchenko <ematirov@gmail.com>
//

#ifndef REMOVEITEMEDITWIDGET_H
#define REMOVEITEMEDITWIDGET_H

#include <QWidget>

class QToolButton;
class QComboBox;
class QModelIndex;

namespace Marble
{

class GeoDataAnimatedUpdate;

class RemoveItemEditWidget: public QWidget
{
    Q_OBJECT

public:
    explicit RemoveItemEditWidget( const QModelIndex& index, QWidget* parent=0 );
    bool editable() const;

Q_SIGNALS:
    void editingDone( const QModelIndex& index );

public Q_SLOTS:
    void setEditable( bool editable );
    void setFeatureIds( const QStringList &ids );
    void setDefaultFeatureId( const QString &featureId );

private Q_SLOTS:
    void save();

private:
    GeoDataAnimatedUpdate* animatedUpdateElement();
    QPersistentModelIndex m_index;
    QToolButton *m_button;
    QComboBox* m_comboBox;
};

} // namespace Marble

#endif

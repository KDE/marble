// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Mihail Ivchenko <ematirov@gmail.com>
//

#ifndef REMOVEITEMEDITWIDGET_H
#define REMOVEITEMEDITWIDGET_H

#include <QWidget>
#include <QPersistentModelIndex>
#include <QToolButton>
#include <QComboBox>

class QModelIndex;

namespace Marble
{

class GeoDataAnimatedUpdate;

class RemoveItemEditWidget: public QWidget
{
    Q_OBJECT

public:
    explicit RemoveItemEditWidget( const QModelIndex& index, QWidget* parent=nullptr );
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

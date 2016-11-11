//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Mihail Ivchenko <ematirov@gmail.com>
//

#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>

#include "RemoveItemEditWidget.h"
#include "MarbleWidget.h"
#include "geodata/data/GeoDataAnimatedUpdate.h"
#include "GeoDataTypes.h"
#include "GeoDataUpdate.h"
#include "GeoDataDelete.h"
#include "MarblePlacemarkModel.h"

namespace Marble {

RemoveItemEditWidget::RemoveItemEditWidget( const QModelIndex &index, QWidget *parent ) :
    QWidget( parent ),
    m_index( index ),
    m_button( new QToolButton ),
    m_comboBox( new QComboBox )
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing( 5 );

    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/icons/remove.png")));
    layout->addWidget( iconLabel );

    QLabel* comboBoxLabel = new QLabel;
    comboBoxLabel->setText( tr( "Choose item:" ) );
    layout->addWidget( comboBoxLabel );

    layout->addWidget( m_comboBox );

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget( m_button );

    setLayout( layout );
}

bool RemoveItemEditWidget::editable() const
{
    return m_button->isEnabled();
}

void RemoveItemEditWidget::setFeatureIds( const QStringList &ids )
{
    QString id = animatedUpdateElement()->update()->getDelete()->first().targetId();
    QString current = m_comboBox->currentIndex() == -1 ? id : m_comboBox->currentText();
    m_comboBox->clear();
    m_comboBox->addItems( ids );
    m_comboBox->setCurrentIndex( m_comboBox->findText( current ) );
}

void RemoveItemEditWidget::setDefaultFeatureId( const QString &featureId )
{
    if( m_comboBox->currentIndex() == -1 ) {
        m_comboBox->setCurrentIndex( m_comboBox->findText( featureId ) );
    }
}

void RemoveItemEditWidget::setEditable( bool editable )
{
    m_button->setEnabled( editable );
}

void RemoveItemEditWidget::save()
{
    animatedUpdateElement()->update()->getDelete()->child(0)->setTargetId( m_comboBox->currentText() );
    emit editingDone(m_index);
}

GeoDataAnimatedUpdate* RemoveItemEditWidget::animatedUpdateElement()
{
    GeoDataObject *object = qvariant_cast<GeoDataObject*>(m_index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    Q_ASSERT( object );
    Q_ASSERT( object->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType );
    return static_cast<GeoDataAnimatedUpdate*>( object );
}

} // namespace Marble

#include "moc_RemoveItemEditWidget.cpp"

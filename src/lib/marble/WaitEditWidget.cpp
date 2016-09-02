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

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QDoubleSpinBox>

#include "WaitEditWidget.h"
#include "MarblePlacemarkModel.h"
#include "GeoDataTypes.h"
#include "GeoDataTourControl.h"
#include "geodata/data/GeoDataWait.h"

namespace Marble
{

WaitEditWidget::WaitEditWidget( const QModelIndex &index, QWidget *parent ) :
    QWidget( parent ),
    m_index( index ),
    m_spinBox( new QDoubleSpinBox ),
    m_button( new QToolButton )
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing( 5 );

    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/marble/player-time.png")));
    layout->addWidget( iconLabel );

    QLabel *waitLabel = new QLabel;
    waitLabel->setText( tr( "Wait duration:" ) );
    layout->addWidget( waitLabel );

    layout->addWidget( m_spinBox );
    m_spinBox->setValue( waitElement()->duration() );
    m_spinBox->setSuffix( tr(" s", "seconds") );

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget( m_button );

    setLayout( layout );
}

bool WaitEditWidget::editable() const
{
    return m_button->isEnabled();
}

void WaitEditWidget::setEditable( bool editable )
{
    m_button->setEnabled( editable );
}

void WaitEditWidget::save()
{
    waitElement()->setDuration( m_spinBox->value() );
    emit editingDone(m_index);
}

GeoDataWait* WaitEditWidget::waitElement()
{
    GeoDataObject *object = qvariant_cast<GeoDataObject*>(m_index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    Q_ASSERT( object );
    Q_ASSERT( object->nodeType() == GeoDataTypes::GeoDataWaitType );
    return static_cast<GeoDataWait*>( object );
}

} // namespace Marble

#include "moc_WaitEditWidget.cpp"

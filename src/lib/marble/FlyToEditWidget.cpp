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

#include <QDoubleSpinBox>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>

#include "FlyToEditWidget.h"
#include "MarbleWidget.h"
#include "geodata/data/GeoDataFlyTo.h"
#include "GeoDataLookAt.h"
#include "GeoDataTypes.h"
#include "GeoDataCamera.h"
#include "MarblePlacemarkModel.h"

namespace Marble
{

FlyToEditWidget::FlyToEditWidget( const QModelIndex &index, MarbleWidget* widget, QWidget *parent ) :
    QWidget( parent ),
    m_widget( widget ),
    m_index( index ),
    m_button( new QToolButton )
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing( 5 );

    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/marble/flag.png")));
    layout->addWidget( iconLabel );

    QHBoxLayout *pairLayout = new QHBoxLayout;
    pairLayout->setSpacing( 10 );

    QHBoxLayout *durationLayout = new QHBoxLayout;
    durationLayout->setSpacing( 5 );

    QLabel *durationLabel = new QLabel;
    durationLabel->setText( tr("Duration:") );
    durationLayout->addWidget( durationLabel );

    m_durationSpin = new QDoubleSpinBox;
    durationLayout->addWidget( m_durationSpin );
    m_durationSpin->setValue( flyToElement()->duration() );
    m_durationSpin->setSuffix( tr(" s", "seconds") );

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addSpacing( 5 );

    QLabel *modeLabel = new QLabel;
    modeLabel->setText( tr("Mode:") );
    modeLayout->addWidget( modeLabel );

    m_modeCombo = new QComboBox;
    modeLayout->addWidget( m_modeCombo );
    m_modeCombo->addItem( tr("Smooth") );
    m_modeCombo->addItem( tr("Bounce") );

    if( flyToElement()->flyToMode() == GeoDataFlyTo::Smooth ){
        m_modeCombo->setCurrentIndex( 0 );
    } else if( flyToElement()->flyToMode() == GeoDataFlyTo::Bounce ){
        m_modeCombo->setCurrentIndex( 1 );
    } else {
        m_modeCombo->setCurrentIndex( -1 );
    }

    pairLayout->addLayout( durationLayout );
    pairLayout->addLayout( modeLayout );

    layout->addLayout( pairLayout );

    QToolButton* flyToPinCenter = new QToolButton;
    flyToPinCenter->setIcon(QIcon(QStringLiteral(":/marble/places.png")));
    flyToPinCenter->setToolTip(tr("Current map center"));
    connect(flyToPinCenter, SIGNAL(clicked()), this, SLOT(updateCoordinates()));
    layout->addWidget(flyToPinCenter);

    m_button->setIcon(QIcon(QStringLiteral(":/marble/document-save.png")));
    connect(m_button, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget( m_button );

    setLayout( layout );
}

bool FlyToEditWidget::editable() const
{
    return m_button->isEnabled();
}

void FlyToEditWidget::setEditable( bool editable )
{
    m_button->setEnabled( editable );
}

void FlyToEditWidget::setFirstFlyTo(const QPersistentModelIndex &index)
{
    if( m_index.internalPointer() == index.internalPointer() ) {
        m_durationSpin->setValue(0);
    }
}

void FlyToEditWidget::updateCoordinates()
{
    m_coord = m_widget->focusPoint();
    m_coord.setAltitude( m_widget->lookAt().range() );
}

void FlyToEditWidget::save()
{
    if (flyToElement()->view() != 0 && m_coord != GeoDataCoordinates()) {
        GeoDataCoordinates coords = m_coord;
        if ( flyToElement()->view()->nodeType() == GeoDataTypes::GeoDataCameraType ) {
            GeoDataCamera* camera = dynamic_cast<GeoDataCamera*>( flyToElement()->view() );
            camera->setCoordinates( coords );
        } else if ( flyToElement()->view()->nodeType() == GeoDataTypes::GeoDataLookAtType ) {
            GeoDataLookAt* lookAt = dynamic_cast<GeoDataLookAt*>( flyToElement()->view() );
            lookAt->setCoordinates( coords );
        } else{
            GeoDataLookAt* lookAt = new GeoDataLookAt;
            lookAt->setCoordinates( coords );
            flyToElement()->setView( lookAt );
        }
    }

    flyToElement()->setDuration(m_durationSpin->value());

    if (m_modeCombo->currentIndex() == 0) {
        flyToElement()->setFlyToMode( GeoDataFlyTo::Smooth );
    } else if (m_modeCombo->currentIndex() == 1) {
        flyToElement()->setFlyToMode( GeoDataFlyTo::Bounce );
    }

    emit editingDone(m_index);
}

GeoDataFlyTo* FlyToEditWidget::flyToElement()
{
    GeoDataObject *object = qvariant_cast<GeoDataObject*>(m_index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    Q_ASSERT( object );
    Q_ASSERT( object->nodeType() == GeoDataTypes::GeoDataFlyToType );
    return static_cast<GeoDataFlyTo*>( object );
}

} // namespace Marble

#include "moc_FlyToEditWidget.cpp"

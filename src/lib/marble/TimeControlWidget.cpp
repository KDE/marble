//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain   <hjain.itbhu@gmail.com>
//

// Own
#include "TimeControlWidget.h"
#include "ui_TimeControlWidget.h"

// Qt
#include <QShowEvent>

// Marble
#include "MarbleClock.h"
#include "MarbleDebug.h"

using namespace Marble;
/* TRANSLATOR Marble::TimeControlWidget */

TimeControlWidget::TimeControlWidget( MarbleClock* clock, QWidget* parent )
    : QDialog( parent ),
      m_uiWidget( new Ui::TimeControlWidget ),
      m_clock( clock ),
      m_lastDateTime()
{
    m_uiWidget->setupUi( this );

    connect( m_uiWidget->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSpeedLabel(int)) );
    connect( m_uiWidget->nowToolButton, SIGNAL(clicked()), this, SLOT(nowClicked()) );
    connect( m_uiWidget->applyButton, SIGNAL(clicked()), this, SLOT(apply()) );
    connect( m_uiWidget->cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
    connect( m_uiWidget->okButton, SIGNAL(clicked()), this, SLOT(apply()) );
    connect( m_uiWidget->okButton, SIGNAL(clicked()), this, SLOT(accept()) );
    connect( m_clock, SIGNAL(timeChanged()), this, SLOT(updateDateTime()) );
    connect( m_clock, SIGNAL(updateIntervalChanged(int)), this, SLOT(updateRefreshRate(int)) );
 
    setModal( false );

}

TimeControlWidget::~TimeControlWidget()
{
    delete m_uiWidget;
}

void TimeControlWidget::updateSpeedLabel( int speed )
{
    m_uiWidget->speedLabel->setText( QString( "%1x" ).arg( speed ) );
}

void TimeControlWidget::updateRefreshRate( int seconds )
{
    m_uiWidget->refreshIntervalSpinBox->setValue( seconds );
}

void TimeControlWidget::updateDateTime()
{
    m_uiWidget->currentDateTimeEdit->setDateTime( m_clock->dateTime().addSecs( m_clock->timezone() ) );
}

void TimeControlWidget::nowClicked()
{
    m_uiWidget->newDateTimeEdit->setDateTime( QDateTime::currentDateTime().toUTC().addSecs( m_clock->timezone() ) );
}

void TimeControlWidget::apply()
{
    if( m_lastDateTime !=  m_uiWidget->newDateTimeEdit->dateTime() )
    {
        m_lastDateTime = m_uiWidget->newDateTimeEdit->dateTime();
        m_clock->setDateTime( m_lastDateTime.toUTC() );
    }
    m_clock->setUpdateInterval( m_uiWidget->refreshIntervalSpinBox->value() );
    m_clock->setSpeed( m_uiWidget->speedSlider->value() );
}

void TimeControlWidget::showEvent(QShowEvent* event)
{
    if( !event->spontaneous() ) 
    {
        // Loading all options
        m_uiWidget->refreshIntervalSpinBox->setValue( m_clock->updateInterval() );
        m_uiWidget->speedSlider->setValue( m_clock->speed() );
        m_uiWidget->speedLabel->setText( QString( "%1x" ).arg( m_clock->speed() ) );
        updateDateTime();
        m_lastDateTime = m_clock->dateTime();
        m_uiWidget->newDateTimeEdit->setDateTime( m_lastDateTime.addSecs( m_clock->timezone() ) );
    }
}

#include "moc_TimeControlWidget.cpp"

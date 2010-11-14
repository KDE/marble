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
#include <QtGui/QShowEvent>

// Marble
#include "MarbleClock.h"
#include "MarbleDebug.h"

using namespace Marble;

TimeControlWidget::TimeControlWidget( MarbleClock* clock, QWidget* parent )
    : QDialog( parent ),
      m_uiWidget( new Ui::TimeControlWidget ),
      m_clock( clock ),
      m_lastDateTime()
{
    m_uiWidget->setupUi( this );
 
    connect( m_uiWidget->speedSlider, SIGNAL( valueChanged( int ) ), this, SLOT( speedChanged( int ) ) );
    connect( m_uiWidget->nowToolButton, SIGNAL( clicked() ), this, SLOT( nowClicked() ) );
    connect( m_uiWidget->applyButton, SIGNAL( clicked() ), this, SLOT( apply() ) );
    connect( m_uiWidget->cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_uiWidget->okButton, SIGNAL( clicked() ), this, SLOT( apply() ) );
    connect( m_uiWidget->okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( m_clock, SIGNAL( timeChanged() ), this, SLOT( updateDateTime() ) );    
 
    setModal( false );

}

TimeControlWidget::~TimeControlWidget()
{
    delete m_uiWidget;
}

void TimeControlWidget::speedChanged( int speed )
{
    m_uiWidget->speedLabel->setText( QString( "%1x" ).arg( speed ) );
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
    m_clock->setSpeed( m_uiWidget->speedSlider->value() );
}

void TimeControlWidget::showEvent(QShowEvent* event)
{
    if( !event->spontaneous() ) 
    {
        // Loading all options
        m_uiWidget->speedSlider->setValue( m_clock->speed() );
        m_uiWidget->speedLabel->setText( QString( "%1x" ).arg( m_clock->speed() ) );
        updateDateTime();
        m_lastDateTime = m_clock->dateTime();
        m_uiWidget->newDateTimeEdit->setDateTime( m_lastDateTime.addSecs( m_clock->timezone() ) );
    }
}

#include "TimeControlWidget.moc"

//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      David Roberts  <dvdr18@gmail.com>
// Copyright 2008      Inge Wallin    <inge@lysator.liu.se>
//

// Own
#include "SunControlWidget.h"
#include "ui_SunControlWidget.h"

// Qt
#include <QtGui/QShowEvent>

#include "ExtDateTime.h"
#include "SunLocator.h"

using namespace Marble;

SunControlWidget::SunControlWidget(QWidget* parent, SunLocator* sunLocator)
    : QDialog( parent ),
      m_uiWidget( new Ui::SunControlWidget ),
      m_sunLocator( sunLocator )
{
    m_uiWidget->setupUi( this );
	
    connect( m_uiWidget->showToolButton,   SIGNAL( clicked( bool ) ),
             this,                        SLOT( showSunClicked( bool ) ) );
    connect( m_uiWidget->centerToolButton, SIGNAL( clicked( bool ) ),
             this,                        SLOT( centerSunClicked( bool ) ) );
    connect( m_uiWidget->nowToolButton,    SIGNAL( clicked( bool ) ), 
             this,                        SLOT( nowClicked( bool ) ) );
    connect( m_uiWidget->sunShadingComboBox, SIGNAL(  currentIndexChanged ( int ) ), 
             this,                          SLOT( showSunShadingClicked( int ) ) );
    connect( m_uiWidget->calendarWidget,   SIGNAL( selectionChanged() ),
             this,                        SLOT( dateChanged() ) );
    connect( m_uiWidget->timeEdit,         SIGNAL( timeChanged( const QTime& ) ),
             this,                        SLOT( timeChanged( const QTime& ) ) );
    connect( m_uiWidget->timeSlider,       SIGNAL( sliderMoved( int ) ),
             this,                        SLOT( hourChanged( int ) ) );
    connect(m_uiWidget->speedSlider, SIGNAL(sliderMoved(int)), this, SLOT(speedChanged(int)));

    setModal( false );

    updateDateTime();
    connect( m_sunLocator->datetime(), SIGNAL( timeChanged() ),
             this,                     SLOT( updateDateTime() ) );
}

SunControlWidget::~SunControlWidget()
{
    delete m_uiWidget;
}

void SunControlWidget::showSunClicked(bool checked)
{
    if ( checked )
        m_uiWidget->showToolButton->setText( tr("&Hide") );
    else
        m_uiWidget->showToolButton->setText( tr("Sh&ow") );

    //m_sunLocator->setShow( checked );
    emit showSun( checked );
}

void SunControlWidget::nowClicked(bool checked)
{
    Q_UNUSED( checked )

    m_sunLocator->datetime()->setNow();
    m_sunLocator->update();
    updateDateTime();
}

void SunControlWidget::showSunShadingClicked( int index )
{
    // Control whether the dark side should be a shadow or the
    // Citylights theme.
    if ( index == 0 )
        m_sunLocator->setCitylights( false );
    else if ( index == 1 )
        m_sunLocator->setCitylights( true );
    m_sunLocator->update();
}

void SunControlWidget::centerSunClicked(bool checked)
{
    m_sunLocator->setCentered(checked);
}

void SunControlWidget::updateDateTime()
{
    QDateTime datetime = m_sunLocator->datetime()->datetime().toLocalTime();
	
    QDate  date     = datetime.date();
    QDate  cur_date = m_uiWidget->calendarWidget->selectedDate();
	
    QTime  time     = datetime.time();
    time = time.addSecs( -time.second() ); // remove seconds
    QTime  cur_time = m_uiWidget->timeEdit->time();
	
    int hour = time.hour();
    int cur_hour = m_uiWidget->timeSlider->value();
	
    // 	mDebug() << "date:" << date << cur_date;
    // 	mDebug() << "time:" << time << cur_time;
	
    if ( date != cur_date )
        m_uiWidget->calendarWidget->setSelectedDate( date );
    if ( time != cur_time )
        m_uiWidget->timeEdit->setTime( time );
    if ( hour != cur_hour )
        m_uiWidget->timeSlider->setValue( time.hour() );
}

void SunControlWidget::timeChanged(const QTime& time)
{
    QDate date = m_uiWidget->calendarWidget->selectedDate();
    m_uiWidget->timeSlider->setValue( time.hour() );
    datetimeChanged( QDateTime( date, time ) );
}

void SunControlWidget::dateChanged()
{
    QDate date = m_uiWidget->calendarWidget->selectedDate();
    QTime time = m_uiWidget->timeEdit->time();
    datetimeChanged( QDateTime( date, time ) );
}

void SunControlWidget::hourChanged(int hour)
{
    QTime time( hour, m_uiWidget->timeEdit->time().minute() );
    m_uiWidget->timeEdit->setTime( time );
}

void SunControlWidget::datetimeChanged(QDateTime datetime)
{
    datetime = datetime.toUTC();
	
    QDateTime cur_datetime = m_sunLocator->datetime()->datetime();
    cur_datetime = cur_datetime.addSecs(-cur_datetime.time().second()); // remove seconds
	
    // 	mDebug() << cur_datetime << datetime;
    if ( cur_datetime == datetime )
        return;
	
    m_sunLocator->datetime()->setDateTime(datetime);
    m_sunLocator->update();
}

void SunControlWidget::speedChanged(int speed)
{
    m_sunLocator->datetime()->setSpeed( speed );
    m_uiWidget->speedLabel->setText( QString( "%1x" ).arg( speed ) );
}

void SunControlWidget::showEvent(QShowEvent* event)
{
    if( !event->spontaneous() ) {
        // Loading all options
        m_uiWidget->speedSlider->setValue( m_sunLocator->datetime()->speed() );
        updateDateTime();
        if( m_sunLocator->getCitylights() )
            m_uiWidget->sunShadingComboBox->setCurrentIndex(1);
        else
            m_uiWidget->sunShadingComboBox->setCurrentIndex(0);
        m_uiWidget->centerToolButton->setChecked( m_sunLocator->getCentered() );
        m_uiWidget->showToolButton->setChecked( m_sunLocator->getShow() );
    }
}

#include "SunControlWidget.moc"

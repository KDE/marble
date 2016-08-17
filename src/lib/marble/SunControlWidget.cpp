//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      David Roberts   <dvdr18@gmail.com>
// Copyright 2008      Inge Wallin     <inge@lysator.liu.se>
// Copyright 2010      Harshit Jain    <hjain.itbhu@gmail.com>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

// Own
#include "SunControlWidget.h"
#include "ui_SunControlWidget.h"

// Qt
#include <QShowEvent>

// Marble
#include "MarbleDebug.h"
#include "MarbleWidget.h"

using namespace Marble;
/* TRANSLATOR Marble::SunControlWidget */

SunControlWidget::SunControlWidget( MarbleWidget* marbleWidget, QWidget* parent )
    : QDialog( parent ),
      m_uiWidget( new Ui::SunControlWidget ),
      m_marbleWidget( marbleWidget ),
      m_shadow( "shadow" )
{
    m_uiWidget->setupUi( this );

    m_uiWidget->lockWarningLabel->hide();
	
    connect( m_uiWidget->applyButton, SIGNAL(clicked()), this, SLOT(apply()) );
    connect( m_uiWidget->cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
    connect( m_uiWidget->okButton, SIGNAL(clicked()), this, SLOT(apply()) );
    connect( m_uiWidget->okButton, SIGNAL(clicked()), this, SLOT(accept()) );
    
    setModal( false );

}

SunControlWidget::~SunControlWidget()
{
    delete m_uiWidget;
}

void SunControlWidget::apply()
{
    if( m_uiWidget->sunShading->isChecked() )
    {
        if( m_uiWidget->showShadow->isChecked() )
        {
            emit showSun( true );
            m_marbleWidget->setShowCityLights( false );
            m_shadow = "shadow";
        }
        else if( m_uiWidget->showNightMap->isChecked() )
        {
            emit showSun( true );
            m_marbleWidget->setShowCityLights( true );
            m_shadow = "nightmap";
        }
    }
    else
    {
        emit showSun( false );
        m_marbleWidget->setShowCityLights( false );
    }

    if( m_uiWidget->lockToSubSolarPointCheckBox->isChecked() )
    {
        m_marbleWidget->setLockToSubSolarPoint( true );
        emit isLockedToSubSolarPoint( true );
    }
    else
    {
        m_marbleWidget->setLockToSubSolarPoint( false );
        emit isLockedToSubSolarPoint( false );
    }

    if( m_uiWidget->subSolarIconCheckBox->isChecked() )
    {
        m_marbleWidget->setSubSolarPointIconVisible( true );
        emit isSubSolarPointIconVisible( true );
    }
    else
    {
        m_marbleWidget->setSubSolarPointIconVisible( false );
        emit isSubSolarPointIconVisible( false );
    }
}

void SunControlWidget::setSunShading( bool active )
{
    m_uiWidget->sunShading->setChecked( active );
}

void SunControlWidget::showEvent( QShowEvent* event )
{
    if( !event->spontaneous() ) 
    {
        // Loading all options
        if( m_marbleWidget->showSunShading() )
        {
            m_uiWidget->sunShading->setChecked( true );
            m_uiWidget->showShadow->setChecked( m_marbleWidget->showSunShading() );
            m_uiWidget->showNightMap->setChecked( m_marbleWidget->showCityLights() );
        }
        else
        {   
            m_uiWidget->showShadow->setChecked( false );
            if (m_shadow == QLatin1String("shadow"))
            {
                m_uiWidget->showShadow->setChecked( true );
            }
            else
            {
                m_uiWidget->showNightMap->setChecked( true );
            }
        }
        m_uiWidget->subSolarIconCheckBox->setChecked( m_marbleWidget->isSubSolarPointIconVisible() );
        m_uiWidget->lockToSubSolarPointCheckBox->setChecked( m_marbleWidget->isLockedToSubSolarPoint() );
    }
}

#include "moc_SunControlWidget.cpp"

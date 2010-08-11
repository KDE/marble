//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      David Roberts  <dvdr18@gmail.com>
// Copyright 2008      Inge Wallin    <inge@lysator.liu.se>
// Copyright 2010      Harshit Jain   <hjain.itbhu@gmail.com>
//

// Own
#include "SunControlWidget.h"
#include "ui_SunControlWidget.h"

// Qt
#include <QtGui/QShowEvent>

// Marble
#include "SunLocator.h"
#include "MarbleDebug.h"

using namespace Marble;

SunControlWidget::SunControlWidget( SunLocator* sunLocator, QWidget* parent )
    : QDialog( parent ),
      m_uiWidget( new Ui::SunControlWidget ),
      m_sunLocator( sunLocator ),
      m_shadow( "shadow" )
{
    m_uiWidget->setupUi( this );
	
    connect( m_uiWidget->applyButton, SIGNAL( clicked() ), this, SLOT( apply() ) );
    connect( m_uiWidget->cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_uiWidget->okButton, SIGNAL( clicked() ), this, SLOT( apply() ) );
    connect( m_uiWidget->okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    
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
            m_sunLocator->setCitylights( false );
            m_sunLocator->update();
            m_shadow = "shadow";
        }
        else if( m_uiWidget->showNightMap->isChecked() )
        {
            emit showSun( true );
            m_sunLocator->setCitylights( true );
            m_sunLocator->update();
            m_shadow = "nightmap";
        }
    }
    else
    {
        emit showSun( false );
        m_sunLocator->setCitylights( false );
        m_sunLocator->update();
    }

    if( m_uiWidget->showZenith->isChecked() )
    {
        m_sunLocator->setCentered( true );
        emit showSunInZenith( true );
    }
    else if( m_uiWidget->hideZenith->isChecked() )
    {
        m_sunLocator->setCentered( false );
        emit showSunInZenith( false );
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
        if( m_sunLocator->getShow() )
        {
            m_uiWidget->sunShading->setChecked( true );
            m_uiWidget->showShadow->setChecked( m_sunLocator->getShow() );
            m_uiWidget->showNightMap->setChecked( m_sunLocator->getCitylights() );
        }
        else
        {   
            m_uiWidget->showShadow->setChecked( false );
            if( m_shadow == "shadow" )
            {
                m_uiWidget->showShadow->setChecked( true );
            }
            else
            {
                m_uiWidget->showNightMap->setChecked( true );
            }
        }
        m_uiWidget->showZenith->setChecked( m_sunLocator->getCentered() );
        m_uiWidget->hideZenith->setChecked( !m_sunLocator->getCentered() );
    }
}

#include "SunControlWidget.moc"

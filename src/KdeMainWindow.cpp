//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

// Own
#include "KdeMainWindow.h"

// KDE
#include <kaction.h>
#include <kactioncollection.h>
#include <kconfigdialog.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kstandardaction.h>

// Local dir
#include "ControlView.h"
#include "MarbleSettingsWidget.h"
#include "marble_part.h"
#include "settings.h"

MainWindow::MainWindow( const QString& marbleDataPath, QWidget *parent )
    : KXmlGuiWindow( parent )
{
    m_part = new MarblePart( this, this, QStringList() << marbleDataPath );

    setCentralWidget( m_part->widget() );

	setupActions();

    setXMLFile( "marbleui.rc" );

    setStandardToolBarMenuEnabled( true );

    insertChildClient( m_part );

    createGUI( 0 );

    setAutoSaveSettings();
}

MainWindow::~MainWindow()
{
    delete m_part;
}


ControlView* MainWindow::marbleControl() const
{
    return m_part->controlView();
}

MarbleWidget* MainWindow::marbleWidget() const
{
    return m_part->controlView()->marbleWidget();
}


void MainWindow::setupActions()
{
    KStandardAction::preferences( this, SLOT( editSettings() ), actionCollection() );
}


void MainWindow::editSettings()
{
	if ( KConfigDialog::showDialog( "settings" ) )
		return; 
 
	KConfigDialog* dialog = new KConfigDialog( this, "settings", MarbleSettings::self() ); 
	MarbleSettingsWidget* confWdg =  
                  new MarbleSettingsWidget( 0 ); 
 
	dialog->setFaceType( KPageDialog::Plain ); 
	dialog->addPage( confWdg, i18n("Marble Settings"), "example" ); 
 
//	connect( dialog, SIGNAL(settingsChanged()), 
//         this, SLOT(updateConfiguration()) ); 
 
	dialog->show();
}

#include "KdeMainWindow.moc"

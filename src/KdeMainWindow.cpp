//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

// Own
#include "KdeMainWindow.h"

// Qt
#include <QtGui/QProgressBar>
#include <QtGui/QToolBar>

// KDE
#include <kaction.h>
#include <kactioncollection.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kxmlguifactory.h>

// GeoData
#include <GeoSceneDocument.h>
#include <GeoSceneHead.h>

// Local dir
#include "MarbleDebug.h"
#include "ControlView.h"
#include "marble_part.h"

namespace Marble
{

MainWindow::MainWindow( const QString& marbleDataPath, QWidget *parent )
    : KXmlGuiWindow( parent )
{
    m_part = new MarblePart( this, this, QVariantList() << marbleDataPath );

    setCentralWidget( m_part->widget() );

    insertChildClient( m_part );

    setXMLFile( "marbleui.rc" );

    setStandardToolBarMenuEnabled( true );

    createGUI( 0 );
    m_part->setupToolBar( toolBars().first() );

    // Creating the plugin menus
    m_part->createInfoBoxesMenu();
    m_part->createOnlineServicesMenu();
	m_part->createFolderList();

    setAutoSaveSettings();

    connect( marbleWidget(), SIGNAL(themeChanged(QString)), 
            this, SLOT(setMapTitle()));
}

MainWindow::~MainWindow()
{
    factory()->removeClient( m_part );
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

void MainWindow::setMapTitle()
{
    GeoSceneDocument *mapTheme = marbleWidget()->mapTheme();
    if ( mapTheme ) {
        setCaption( tr( mapTheme->head()->name().toLatin1() ) );
    }
}

}

#include "KdeMainWindow.moc"

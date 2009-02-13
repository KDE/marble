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

// Qt
#include <QtGui/QProgressBar>

// KDE
#include <kaction.h>
#include <kactioncollection.h>
#include <kstatusbar.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>

#include <QtCore/QDebug>

// GeoData
#include <GeoSceneDocument.h>
#include <GeoSceneHead.h>

// Local dir
#include "ControlView.h"
#include "HttpDownloadManager.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "marble_part.h"

namespace Marble
{

MainWindow::MainWindow( const QString& marbleDataPath, QWidget *parent )
    : KXmlGuiWindow( parent )
{
    m_part = new MarblePart( this, this, QStringList() << marbleDataPath );

    setCentralWidget( m_part->widget() );

    insertChildClient( m_part );

    setXMLFile( "marbleui.rc" );

    setStandardToolBarMenuEnabled( true );

    createGUI( 0 );

    m_part->createInfoBoxesMenu();

    setAutoSaveSettings();

    connect( marbleWidget(), SIGNAL( themeChanged( QString ) ), 
	     this, SLOT( setMapTitle() ) );
    initStatusBar();
}

MainWindow::~MainWindow()
{
    delete m_part;
}

void MainWindow::initStatusBar()
{
    initDownloadProgressBar();
}

void MainWindow::initDownloadProgressBar()
{
    // get status bar and add progress widget
    KStatusBar * const status_bar = statusBar();
    qDebug() << "got status bar:" << status_bar;
    m_downloadProgressBar = new QProgressBar;
    status_bar->addPermanentWidget( m_downloadProgressBar );

    HttpDownloadManager * const downloadManager =
        m_part->controlView()->marbleWidget()->map()->model()->downloadManager();
    qDebug() << "got download manager:" << downloadManager;

    connect( downloadManager, SIGNAL( jobAdded( int )),
             this, SLOT( downloadProgressJobAdded( int )));
    connect( downloadManager, SIGNAL( downloadComplete( QString, QString )),
             this, SLOT( downloadProgressJobCompleted( QString, QString )));
}

QProgressBar* MainWindow::downloadProgressBar() const
{
    return m_downloadProgressBar;
}

void MainWindow::downloadProgressJobAdded( int totalJobs )
{
    if ( m_downloadProgressBar->value() < 0 ) {
        m_downloadProgressBar->setMaximum( 1 );
        m_downloadProgressBar->setValue( 0 );
    } else {
        m_downloadProgressBar->setMaximum( m_downloadProgressBar->maximum() + 1 );
    }
    qDebug() << "downloadProgressJobAdded: value/maximum: "
             << m_downloadProgressBar->value() << '/' << m_downloadProgressBar->maximum();
}

void MainWindow::downloadProgressJobCompleted( QString, QString )
{
    m_downloadProgressBar->setValue( m_downloadProgressBar->value() + 1 );
    if ( m_downloadProgressBar->value() == m_downloadProgressBar->maximum() )
        m_downloadProgressBar->reset();

    qDebug() << "downloadProgressJobCompleted: value/maximum: "
             << m_downloadProgressBar->value() << '/' << m_downloadProgressBar->maximum();
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

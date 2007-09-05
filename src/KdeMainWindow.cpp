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

#include "KdeMainWindow.h"

#include "ControlView.h"
#include "marble_part.h"

#include <kparts/part.h>
#include <kparts/componentfactory.h>

MainWindow::MainWindow( const QString& marbleDataPath, QWidget *parent )
    : KXmlGuiWindow( parent )
{
    m_part = new MarblePart( this, this, QStringList() << marbleDataPath );

    setCentralWidget( m_part->widget() );

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

#include "KdeMainWindow.moc"

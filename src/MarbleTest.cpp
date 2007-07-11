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

#include "MarbleTest.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>

#include <lib/MarbleWidget.h>

MarbleTest::MarbleTest( MarbleWidget* marbleWidget )
    : m_marbleWidget(marbleWidget)
{
}

void MarbleTest::timeDemo()
{
    m_marbleWidget->zoomView( 1500 );
//    m_marbleWidget->resize( 800, 600 );

    qDebug() << "Starting Performance Test";
    QTime t;

    t.start();

    for ( int j = 0; j < 10; ++j ) {
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft();
        }
    }

    qDebug( "Timedemo finished in %ims", t.elapsed() );
    qDebug() <<  QString("= %1 fps").arg(200*1000/(double)(t.elapsed()));
/*
    m_marbleWidget->setMapTheme( "bluemarble/bluemarble.dgml" );
    m_marbleWidget->setShowScaleBar( false );
    m_marbleWidget->setShowWindRose( false );
    m_marbleWidget->setShowGrid( false );
    m_marbleWidget->setShowPlaces( false );
    m_marbleWidget->setShowPlaces( false );
    m_marbleWidget->setShowBorders( false );
    m_marbleWidget->setShowRivers( false );
    m_marbleWidget->setShowLakes( false );

    t.restart();

    for ( int j = 0; j < 10; ++j ) {
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft();
        }
    }

    qDebug( "Timedemo finished in %ims", t.elapsed() );
    qDebug() <<  QString("= %1 fps").arg(200*1000/(double)(t.elapsed()));
*/
}

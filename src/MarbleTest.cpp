//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "MarbleTest.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include <global.h>
#include <MarbleDebug.h>
#include <MarbleWidget.h>
#include <MarbleModel.h>

using namespace Marble;

MarbleTest::MarbleTest( MarbleWidget* marbleWidget )
    : m_marbleWidget(marbleWidget)
{
}

void MarbleTest::timeDemo()
{
    m_marbleWidget->zoomView( 1500 );
    m_marbleWidget->setMapThemeId( "earth/srtm/srtm.dgml" );
    m_marbleWidget->setMapQuality( Marble::NormalQuality );
//    m_marbleWidget->resize( 800, 600 );
    m_marbleWidget->centerOn( 9.4, 54.8 );

    QMessageBox::information(m_marbleWidget, QString( "Marble Speed Test" ), QString( "Press Ok to start test" ) );

    QTime t;
    //m_marbleWidget->setMapTheme( "plain/plain.dgml" );
    //m_marbleWidget->setMapTheme( "bluemarble/bluemarble.dgml" );

/*
    m_marbleWidget->setShowGrid( false );
    m_marbleWidget->setShowPlaces( false );
    m_marbleWidget->setShowBorders( false );
    m_marbleWidget->setShowRivers( false );
    m_marbleWidget->setShowLakes( false );
*/

        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight();
            QCoreApplication::flush();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft();
            QCoreApplication::flush();
        }

    qDebug() << "Starting Performance Test";

    t.start();

    for ( int j = 0; j < 10; ++j ) {
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight();
            QCoreApplication::flush();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft();
            QCoreApplication::flush();
        }
    }

    qDebug( "Timedemo finished in %ims", t.elapsed() );
    qDebug() <<  QString("= %1 fps").arg(200*1000/(qreal)(t.elapsed()));

}

void MarbleTest::gpsDemo()
{
    //
    //set up for the test
    //
    m_marbleWidget->centerOn( -15.2325, 58.3723 );
    
    m_marbleWidget->setShowGps( true );
    
    //get the gpx file
    QString fileName = QFileDialog::getOpenFileName(m_marbleWidget,
            "Open File", QString(), 
            "GPS Data (*.gpx);;KML (*.kml)");
    
    if ( ! fileName.isNull() ) {
        QString extension = fileName.section( '.', -1 );

        if ( extension.compare( "gpx", Qt::CaseInsensitive ) == 0 ) {
            m_marbleWidget->openGpxFile( fileName );
        }
    }
   
    QTime t;
    QTime totalTime;
    
    QVector<int> movingStats;
    QVector<int> staticStats;
    int temp=0;
    int totalMoving =0;
    int totalStatic =0;
    
    //
    //Start the test
    //
    totalTime.start();
    for( int i = 0; i< 5 ; i++) {
        
        m_marbleWidget->zoomViewBy( 400 );
        totalMoving =0;
        totalStatic =0;
        m_marbleWidget->centerOn( -15.2325, 58.3723 );
    
        for( int i = 0; i< 5 ;i++ ){
            if( i%2) {
                m_marbleWidget->moveLeft();
            } else {
                m_marbleWidget->moveRight();
            }
            t.start();
            m_marbleWidget->updateGps();
            temp = t.elapsed();
//             qDebug("time elapsed moving %d",temp);
            totalMoving += temp;
            
            
        }
        
        for( int i = 0; i< 10 ;i++ ){
            t.start();
            m_marbleWidget->updateGps();
            temp=t.elapsed();
//             qDebug("time elapsed static %d",t.elapsed());
            totalStatic+=temp;
        }
        
//         qDebug ( "average of moving is %d at %d zoom", 
//                  (totalMoving/10), m_marbleWidget->zoom() );
//         qDebug( "average of static is %d at %d zoom", 
//                 (totalStatic/10) , m_marbleWidget->zoom() );
        movingStats.append( totalMoving/10 );
        staticStats.append( totalStatic/10 );
        
    }
    qDebug("total test time: %d", totalTime.elapsed());
    
    qDebug("full moving stats: ") ;
    qDebug() << movingStats;
    
    qDebug("full static stats: ");
    qDebug() << staticStats;
    
    
}

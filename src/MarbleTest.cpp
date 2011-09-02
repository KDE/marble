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
    QMessageBox::information(m_marbleWidget, QString( "Marble Speed Test" ), QString( "Press Ok to start test" ) );

    m_marbleWidget->setViewContext( Marble::Animation );

        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight( Instant );
            QCoreApplication::flush();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft( Instant );
            QCoreApplication::flush();
        }

    qDebug() << "Starting Performance Test";

    QTime t;
    t.start();

    for ( int j = 0; j < 10; ++j ) {
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight( Instant );
            QCoreApplication::flush();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft( Instant );
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
    
    //get the gpx file
    QString fileName = QFileDialog::getOpenFileName(m_marbleWidget,
            "Open File", QString(), 
            "GPS Data (*.gpx);;KML (*.kml)");
    
    if ( ! fileName.isNull() ) {
        m_marbleWidget->model()->addGeoDataFile( fileName );
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
//            m_marbleWidget->updateGps();
            temp = t.elapsed();
//             qDebug("time elapsed moving %d",temp);
            totalMoving += temp;
            
            
        }
        
        for( int i = 0; i< 10 ;i++ ){
            t.start();
//            m_marbleWidget->updateGps();
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

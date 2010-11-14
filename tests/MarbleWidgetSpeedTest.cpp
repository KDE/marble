//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2007      Tim Sutton   <tim@linfiniti.com>
//

#include <QtGui/QtGui> //needed because this is a gui test
#include <QtTest/QtTest>
#include <QtCore/QtCore>
#include <QtCore/QTime>
#include "MarbleDirs.h"
#include "MarbleWidget.h"

namespace Marble
{

class MarbleWidgetSpeedTest: public QObject
{
  Q_OBJECT
  private slots:
  void timeTest();
  void initTestCase();// will be called before the first testfunction is executed.
  void cleanupTestCase();// will be called after the last testfunction was executed.
  void init(){};// will be called before each testfunction is executed.
  void cleanup(){};// will be called after every testfunction.
  private:
  MarbleWidget *m_marbleWidget;
};

void MarbleWidgetSpeedTest::initTestCase()
{
    MarbleDirs::setMarbleDataPath( DATA_PATH );
    MarbleDirs::setMarblePluginPath( PLUGIN_PATH );
    m_marbleWidget = new MarbleWidget();
    m_marbleWidget->show();
}
void MarbleWidgetSpeedTest::cleanupTestCase()
{
    delete m_marbleWidget;
}
void MarbleWidgetSpeedTest::timeTest()
{
    m_marbleWidget->setMapThemeId( "plain/plain.dgml" );
    m_marbleWidget->zoomView( 1500 );
//    m_marbleWidget->resize( 800, 600 );

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

    //required maximum elapsed time for test to pass
    QVERIFY(t.elapsed() < 10);
    //required frames per second for test to pass 
    //redundant with above really but I leave it in
    //for now...
    unsigned int fps = (uint)( 200.0*1000.0/(double)(t.elapsed()) );
    QVERIFY(fps > 10);

}

}

//QTEST_MAIN(MarbleWidgetSpeedTest)
int main( int argc, char ** argv )
{
    QApplication app( argc, argv );
    Marble::MarbleWidgetSpeedTest speedTest;
    QTest::qExec( &speedTest );
    return 0;
}

#include "MarbleWidgetSpeedTest.moc"

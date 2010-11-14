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

#include "GeoPolygon.h"

namespace Marble
{

class GeoPolygonTest: public QObject
{
  Q_OBJECT
  private slots:
  void pointLoadTest();
  void initTestCase();// will be called before the first testfunction is executed.
  void cleanupTestCase();// will be called after the last testfunction was executed.
  void init(){};// will be called before each testfunction is executed.
  void cleanup(){};// will be called after every testfunction.
  private:
};

void GeoPolygonTest::initTestCase()
{
}
void GeoPolygonTest::cleanupTestCase()
{
}
void GeoPolygonTest::pointLoadTest()
{
  PntMap myPntMap;
  myPntMap.load(""); //null filename test - marble should not crash!!!
}

}

QTEST_MAIN(Marble::GeoPolygonTest) 
#include "GeoPolygonTest.moc"

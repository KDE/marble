//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include <QtCore/QObject>
#include <QtCore/QTemporaryFile>
#include <QtTest/QTest>
#include <QtCore/QDebug>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneZoom.h"
#include "GeoWriter.h"

using namespace Marble;

class TestGeoSceneWriter : public QObject
{
    Q_OBJECT

private slots:
    void writeHeadTag();
};

void TestGeoSceneWriter::writeHeadTag()
{
    GeoSceneDocument *document = new GeoSceneDocument;
    
    GeoSceneHead* head = document->head();
    head->setName( "Test Map" );
    head->setTheme( "testmap" );
    head->setTarget( "earth" );
    
    GeoSceneZoom* zoom = document->head()->zoom();
    zoom->setMaximum( 1000 );
    zoom->setMaximum( 500 );
    zoom->setDiscrete( true );
    
    QTemporaryFile file;
    file.open();
    
    GeoWriter writer;
    writer.setDocumentType( "http://edu.kde.org/marble/dgml/2.0" );
    QVERIFY( writer.write( &file, head ) );
}

QTEST_MAIN( TestGeoSceneWriter )
#include "TestGeoSceneWriter.moc"
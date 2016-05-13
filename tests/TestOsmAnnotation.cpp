//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include <QObject>

#include "PlacemarkTextAnnotation.h"
#include "TextAnnotation.h"

using namespace Marble;

class TestOsmAnnotation : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void saveAnnotations_data();
    void saveAnnotations();
private:
    QMap< QString, QSharedPointer<TextAnnotation> > textAnnotations;
};

void TestOsmAnnotation::initTestCase()
{
//    PlacemarkTextAnnotation* standard;
//    uncomment the following to get the Undefined references
//    standard = new PlacemarkTextAnnotation();
//    standard->setName( "Standard Annon" );
}

void TestOsmAnnotation::saveAnnotations_data()
{

}

void TestOsmAnnotation::saveAnnotations()
{
    //Make some annotations and save them
}

QTEST_MAIN( TestOsmAnnotation)
#include "TestOsmAnnotation.moc"


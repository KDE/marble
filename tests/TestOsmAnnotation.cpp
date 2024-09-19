// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
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
    QMap<QString, QSharedPointer<TextAnnotation>> textAnnotations;
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
    // Make some annotations and save them
}

QTEST_MAIN(TestOsmAnnotation)
#include "TestOsmAnnotation.moc"

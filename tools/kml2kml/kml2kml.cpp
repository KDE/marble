//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

// A simple tool to read a .kml file and write it back to a new .kml file
// Mainly useful to test the successful reading and writing of KML data

#include <MarbleWidget.h>
#include <ParsingRunnerManager.h>
#include <PluginManager.h>
#include <GeoWriter.h>

#include <QApplication>
#include <QFile>
#include <QDebug>

using namespace std;
using namespace Marble;

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    QString inputFilename;
    int inputIndex = app.arguments().indexOf( "-i" );
    if ( inputIndex > 0 && inputIndex + 1 < argc ) {
        inputFilename = app.arguments().at( inputIndex + 1 );
    } else {
        qDebug( " Syntax: kml2kml -i sourcefile [-o kml-targetfile]" );
        return 1;
    }

    QString outputFilename = "output.kml";
    int outputIndex = app.arguments().indexOf("-o");
    if ( outputIndex > 0 && outputIndex + 1 < argc )
        outputFilename = app.arguments().at( outputIndex + 1 );

    ParsingRunnerManager* manager = new ParsingRunnerManager( new PluginManager );
    GeoDataDocument* document = manager->openFile( inputFilename );
    if (!document) {
        qDebug() << "Could not parse input file. No error message available unfortunately";
        return 2;
    }


    QFile output(outputFilename);
    if (!output.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to write to " << output.fileName();
        return 3;
    }

    GeoWriter().write(&output, dynamic_cast<GeoDataFeature*>(document));
}

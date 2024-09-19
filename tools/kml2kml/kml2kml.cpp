// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

// A simple tool to read a .kml file and write it back to a new .kml file
// Mainly useful to test the successful reading and writing of KML data

#include <GeoDataDocumentWriter.h>
#include <MarbleWidget.h>
#include <ParsingRunnerManager.h>
#include <PluginManager.h>

#include <QApplication>
#include <QDebug>
#include <QFile>

using namespace std;
using namespace Marble;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QString inputFilename;
    int inputIndex = app.arguments().indexOf("-i");
    if (inputIndex > 0 && inputIndex + 1 < argc) {
        inputFilename = app.arguments().at(inputIndex + 1);
    } else {
        qDebug(" Syntax: kml2kml -i sourcefile [-o kml-targetfile]");
        return 1;
    }

    QString outputFilename = "output.kml";
    int outputIndex = app.arguments().indexOf("-o");
    if (outputIndex > 0 && outputIndex + 1 < argc)
        outputFilename = app.arguments().at(outputIndex + 1);

    ParsingRunnerManager *manager = new ParsingRunnerManager(new PluginManager);
    GeoDataDocument *document = manager->openFile(inputFilename);
    if (!document) {
        qDebug() << "Could not parse input file. No error message available unfortunately";
        return 2;
    }

    GeoDataDocumentWriter::write(outputFilename, *document);
}

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
#include <GeoDataParser.h>
#include <GeoWriter.h>

#include <QtCore/QFile>
#include <iostream>

using namespace std;
using namespace Marble;

int main(int argc, char** argv)
{
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " input.kml output.kml" << endl;
  }

  QFile file(argv[1]);
  file.open(QIODevice::ReadOnly);
  GeoDataParser parser(GeoData_KML);
  if ( !parser.read( &file ) ) {
    cerr << "Error parsing '" << file.fileName().toStdString();
    cerr << "': '" << parser.errorString().toStdString() << "'" << endl;
    return 1;
  }

  GeoDocument* document = parser.releaseDocument();
  if (!document) {
    cerr << "Could not parse kml file. No error message available unfortunately" << endl;
    return 2;
  }

  QFile output(argv[2]);
  if (!output.open(QIODevice::WriteOnly)) {
    cerr << "Unable to write to " << output.fileName().toStdString() << endl;
    return 3;
  }

  GeoWriter().write(&output, dynamic_cast<GeoDataFeature*>(document));
}

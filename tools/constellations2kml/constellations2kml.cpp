//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//


#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QStringList>
#include <QHash>
#include <QPair>

#include <cmath>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QFile file( "constellations.kml" );
    file.open( QIODevice::WriteOnly );
    QTextStream out( &file );

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n"
        << "<kml xmlns=\"http://earth.google.com/kml/2.2\" hint=\"target=sky\"> \n"
        << "<Document> \n"
        << "    <Style id=\"lineStyle1\"> \n"
        << "        <LineStyle> \n"
        << "            <color>ffffffff</color> \n"
        << "        </LineStyle> \n"
        << "    </Style> \n"
        << "    <Style id=\"lineStyle2\"> \n"
        << "        <LineStyle> \n"
        << "            <color>ffff0000</color> \n"
        << "        </LineStyle> \n"
        << "    </Style> \n";

    QFile starsData( "catalog.dat" );
    QFile constellationsData( "constellations.dat" );

    if ( starsData.open( QFile::ReadOnly ) && constellationsData.open( QFile::ReadOnly ) ) {

        QTextStream streamConstellations( &constellationsData );
        QTextStream streamStars( &starsData );

        QHash<int, QPair<qreal, qreal> > hash;

        QString starsLine;
        int index;
        qreal longitude;
        qreal latitude;
        QPair<qreal, qreal> pair;
        do {
            starsLine = streamStars.readLine();
            index = starsLine.mid( 0, 4 ).toInt();

            QString recString = starsLine.mid( 75, 6 );
            double raHH = recString.mid( 0, 2 ).toDouble();
            double raMM = recString.mid( 2, 2 ).toDouble();
            double raSS = recString.mid( 4, 2 ).toDouble();

            longitude = ( raHH + raMM / 60.0  +raSS / 3600.0 ) * 15.0 - 180.0;

            QString decString = starsLine.mid( 83, 7 );
            double deSign = ( decString.mid( 0, 1 ) == "-" ) ? -1.0 : 1.0;
            double deHH = decString.mid( 1, 2 ).toDouble();
            double deMM = decString.mid( 3, 2 ).toDouble();
            double deSS = decString.mid( 5, 2 ).toDouble();

            double deValue = deSign * ( deHH + deMM / 60.0 + deSS / 3600.0 ) / 180.0 * M_PI;

            latitude = deValue * 180.0 / M_PI;

            pair.first = longitude;
            pair.second = latitude;

            hash.insert( index, pair );
        } while ( !starsLine.isNull() );

        QString name;
        QString indexList;
        QStringList starIndexes;
        while ( !streamConstellations.atEnd() ) {
            name = streamConstellations.readLine();

            // Check for null line at end of file
            if ( name.isNull() ) {
                continue;
            }

            // Ignore Comment lines in header and
            // between constellation entries
            if ( name.startsWith( '#' ) )    {
                continue;
            }

            indexList = streamConstellations.readLine();

            // Make sure we have a valid name and indexList
            if ( indexList.isNull() ) {
                break;
            }

            out << "    <Placemark> \n"
                << "        <styleUrl>#lineStyle1</styleUrl> \n"
                << "        <MultiGeometry> \n"
                << "            <LineString> \n"
                << "                <coordinates> \n";

            starIndexes = indexList.split( " " );
            for ( int i = 0; i < starIndexes.size(); ++i ) {
                if ( starIndexes.at(i) == "-1" ) {
                    out << "                </coordinates> \n"
                        << "            </LineString> \n"
                        << "            <LineString> \n"
                        << "                <coordinates> \n";
                } else if ( starIndexes.at(i) == "-2" ) {
                    out << "                </coordinates> \n"
                        << "            </LineString> \n"
                        << "        </MultiGeometry> \n"
                        << "    </Placemark> \n"
                        << "    <Placemark> \n"
                        << "        <styleUrl>#lineStyle2</styleUrl> \n"
                        << "        <MultiGeometry> \n"
                        << "            <LineString> \n"
                        << "                <coordinates> \n";
                } else if ( starIndexes.at(i) == "-3" ) {
                    out << "                </coordinates> \n"
                        << "            </LineString> \n"
                        << "        </MultiGeometry> \n"
                        << "    </Placemark> \n"
                        << "    <Placemark> \n"
                        << "        <styleUrl>#lineStyle1</styleUrl> \n"
                        << "        <MultiGeometry> \n"
                        << "            <LineString> \n"
                        << "                <coordinates> \n";
                }
                QHash<int, QPair<qreal, qreal> >::const_iterator j = hash.find( starIndexes.at(i).toInt() );
                while( j != hash.end() && j.key() == starIndexes.at(i).toInt() ) {
                    out << "                " << j.value().first << "," << j.value().second << " \n";
                    ++j;
                }
            }

            out << "                </coordinates> \n"
                << "            </LineString> \n"
                << "        </MultiGeometry> \n"
                << "    </Placemark> \n";
        }
    }

    out << "</Document> \n"
        << "</kml> \n";

    constellationsData.close();
    starsData.close();
    file.close();

    app.exit();
}


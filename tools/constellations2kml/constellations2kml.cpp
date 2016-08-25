//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "MarbleGlobal.h"

#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QStringList>
#include <QHash>
#include <QPair>

#include <cmath>

using namespace Marble;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QFile file( "constellations.kml" );
    file.open( QIODevice::WriteOnly );
    QTextStream out( &file );

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n"
        << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" hint=\"target=sky\"> \n"
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
        << "    </Style> \n"
        << "    <Style id=\"iconStyle\"> \n"
        << "        <IconStyle> \n"
        << "            <Icon> \n"
        << "                <href></href> \n"
        << "            </Icon> \n"
        << "        </IconStyle> \n"
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

            longitude = ( raHH + raMM / 60.0 + raSS / 3600.0 ) * 15.0 - 180.0;

            QString decString = starsLine.mid( 83, 7 );
            double deSign = decString.startsWith(QLatin1Char('-')) ? -1.0 : 1.0;
            double deHH = decString.mid( 1, 2 ).toDouble();
            double deMM = decString.mid( 3, 2 ).toDouble();
            double deSS = decString.mid( 5, 2 ).toDouble();

            double deValue = deSign * ( deHH + deMM / 60.0 + deSS / 3600.0 );

            latitude = deValue;

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
            if (name.startsWith(QLatin1Char('#'))) {
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

            starIndexes = indexList.split(QLatin1Char(' '));
            QList<qreal> x;
            QList<qreal> y;
            QList<qreal> z;
            int numberOfStars = 0;
            for ( int i = 0; i < starIndexes.size(); ++i ) {
                if (starIndexes.at(i) == QLatin1String("-1")) {
                    out << "                </coordinates> \n"
                        << "            </LineString> \n"
                        << "            <LineString> \n"
                        << "                <coordinates> \n";
                } else if (starIndexes.at(i) == QLatin1String("-2")) {
                    out << "                </coordinates> \n"
                        << "            </LineString> \n"
                        << "        </MultiGeometry> \n"
                        << "    </Placemark> \n"
                        << "    <Placemark> \n"
                        << "        <styleUrl>#lineStyle2</styleUrl> \n"
                        << "        <MultiGeometry> \n"
                        << "            <LineString> \n"
                        << "                <coordinates> \n";
                } else if (starIndexes.at(i) == QLatin1String("-3")) {
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
                QHash<int, QPair<qreal, qreal> >::const_iterator j = hash.constFind( starIndexes.at(i).toInt() );
                while( j != hash.constEnd() && j.key() == starIndexes.at(i).toInt() ) {
                    out << "                " << j.value().first << "," << j.value().second << " \n";
                    x.append( cos( j.value().first * DEG2RAD ) * cos( j.value().second * DEG2RAD ) );
                    y.append( sin( j.value().first * DEG2RAD ) * cos( j.value().second * DEG2RAD ) );
                    z.append( sin( j.value().second * DEG2RAD ) );
                    ++numberOfStars;
                    ++j;
                }
            }

            out << "                </coordinates> \n"
                << "            </LineString> \n"
                << "        </MultiGeometry> \n"
                << "    </Placemark> \n";

            qreal xMean = 0.0;
            qreal yMean = 0.0;
            qreal zMean = 0.0;
            for ( int s = 0; s < numberOfStars; ++s ) {
                xMean += x.at(s);
                yMean += y.at(s);
                zMean += z.at(s);
            }

            xMean = xMean / numberOfStars;
            yMean = yMean / numberOfStars;
            zMean = zMean / numberOfStars;

            qreal labelLongitude = RAD2DEG * atan2( yMean, xMean );
            qreal labelLatitude = RAD2DEG * atan2( zMean, sqrt( xMean * xMean + yMean * yMean ) );

            out << "    <Placemark> \n"
                << "        <styleUrl>#iconStyle</styleUrl> \n"
                << "        <name>" << name << "</name> \n"
                << "        <Point> \n"
                << "            <coordinates>" << labelLongitude << "," << labelLatitude << "</coordinates> \n"
                << "        </Point> \n"
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


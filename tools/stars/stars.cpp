//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//


#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDataStream>

#include <cmath>
#include <iostream>

#define ENABLEGUI

// Set up Color Table Per B-V Color indices from some Reference Stars
QVector<double> colorTable( 0 );

void exportToDat()
{
    QFile file("stars.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    // Write a header with a "magic number" and a version
    out << (quint32)0x73746172;
    out << (qint32)004;

    out.setVersion(QDataStream::Qt_4_3);

    QFile data("catalog.dat");
    if ( data.open( QFile::ReadOnly ) ) {
        QTextStream stream(&data);
        QString line;
        do {
            line = stream.readLine();

            QString idString = line.mid(0,4);
            int idValue = idString.toInt();

            QString recString = line.mid( 75, 6 );

            double raHH = recString.mid( 0, 2 ).toDouble();
            double raMM = recString.mid( 2, 2 ).toDouble();
            double raSS = recString.mid( 4, 2 ).toDouble();

            double raValue = 15 * ( raHH + raMM / 60.0 + raSS / 3600.0 ) / 180.0 * M_PI;

            QString decString = line.mid( 83, 7 );

            double deSign = decString.startsWith(QLatin1Char('-')) ? -1.0 : 1.0;
            double deHH = decString.mid( 1, 2 ).toDouble();
            double deMM = decString.mid( 3, 2 ).toDouble();
            double deSS = decString.mid( 5, 2 ).toDouble();

            double deValue = deSign * ( deHH + deMM / 60.0 + deSS / 3600.0 ) / 180.0 * M_PI;

            QString magString = line.mid( 102, 5 );
            double magValue = magString.toDouble();

            QString bvString = line.mid( 108, 6);
            int     colorIdx = 2; // Default White

            // Find Index of Table Entry with Closest B-V value (Smallest Difference)
            if (bvString != QLatin1String("      ")) {
                double bvValue = bvString.toDouble();
                double bvMinDifference = fabs(colorTable.at(0)-bvValue);
                for (int i = 1; i < colorTable.size(); ++i) {
                    double bvDifference = fabs(colorTable.at(i)-bvValue);
                    if (bvDifference < bvMinDifference) {
                        colorIdx = i;
                        bvMinDifference = bvDifference;
                    }
                }
            }


//            qDebug() << "Rec:" << recString << "Dec.:" << decString << "Mag.:" << magString;
            if ( !line.isNull() && magValue < 6.0 ) {
            if (raValue != 0 && deValue != 0) { // Filter out Novae and DSOs
            if (idValue != 5958) { // Filter out special cases, like novae ( T CrB, ... )
            qDebug() << "ID:" << idValue << "RA:" << raValue << "DE:" << deValue << "mag:" << magValue << "B-V:" << bvString << "idx:" << colorIdx;
            out << idValue;
            out << raValue;
            out << deValue;
            out << magValue;
            out << colorIdx;
            }
        }
            }
        } while ( !line.isNull() );
    }
    file.flush();
}

void exportToKml()
{
    QFile file("kmlsky.kml");
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n"
        << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" hint=\"target=sky\"> \n"
        << "<Document> \n"
        << "   <Style id=\"mag-1 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_0_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_1_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_2_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_3_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_4_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_5_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_6_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_7_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag7 blue\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_8_blue.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag-1 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_0_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_1_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_2_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_3_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_4_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_5_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_6_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_7_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag7 bluewhite\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_8_bluewhite.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag-1 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_0_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_1_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_2_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_3_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_4_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_5_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_6_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_7_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag7 white\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_8_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag-1 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_0_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_1_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_2_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_3_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_4_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_5_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_6_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_7_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag7 yellow\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_8_yellow.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag-1 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_0_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_1_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_2_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_3_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_4_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_5_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_6_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_7_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag7 orange\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_8_orange.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag-1 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_0_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_1_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_2_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_3_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_4_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_5_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_6_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_7_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag7 red\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_8_red.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag-1 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_0_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_1_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_2_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_3_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_4_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_5_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_6_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_7_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag7 garnetred\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>data/star_8_garnetred.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n";

    QFile data("catalog.dat");
    if ( data.open( QFile::ReadOnly ) ) {
        QTextStream stream(&data);
        QString line;
        do {
            line = stream.readLine();

            QString recString = line.mid( 75, 6 );
            double raHH = recString.mid( 0, 2 ).toDouble();
            double raMM = recString.mid( 2, 2 ).toDouble();
            double raSS = recString.mid( 4, 2 ).toDouble();

            qreal longitude = ( raHH + raMM / 60.0 + raSS / 3600.0 ) * 15.0 - 180.0;

            QString decString = line.mid( 83, 7 );
            double deSign = decString.startsWith(QLatin1Char('-')) ? -1.0 : 1.0;
            double deHH = decString.mid( 1, 2 ).toDouble();
            double deMM = decString.mid( 3, 2 ).toDouble();
            double deSS = decString.mid( 5, 2 ).toDouble();

            double deValue = deSign * ( deHH + deMM / 60.0 + deSS / 3600.0 );

            qreal latitude = deValue;

            QString magString = line.mid( 102, 5 );
            double magValue = magString.toDouble();

            QString bvString = line.mid( 108, 6);
            int     colorIdx = 2; // Default White

            // Find Index of Table Entry with Closest B-V value (Smallest Difference)
            if (bvString != QLatin1String("      ")) {
                double bvValue = bvString.toDouble();
                double bvMinDifference = fabs(colorTable.at(0)-bvValue);
                for (int i = 1; i < colorTable.size(); ++i) {
                    double bvDifference = fabs(colorTable.at(i)-bvValue);
                    if (bvDifference < bvMinDifference) {
                        colorIdx = i;
                        bvMinDifference = bvDifference;
                    }
                }
            }

            QString styleId;
            if ( magValue < -1 ) {
                styleId = "mag-1";
            }
            else if ( magValue < 0 && magValue > -1 ) {
                styleId = "mag0";
            }
            else if ( magValue < 1 && magValue > 0 ) {
                styleId = "mag1";
            }
            else if ( magValue < 2 && magValue > 1 ) {
                styleId = "mag2";
            }
            else if ( magValue < 3 && magValue > 2 ) {
                styleId = "mag3";
            }
            else if ( magValue < 4 && magValue > 3 ) {
                styleId = "mag4";
            }
            else if ( magValue < 5 && magValue > 4 ) {
                styleId = "mag5";
            }
            else if ( magValue < 6 && magValue > 5 ) {
                styleId = "mag6";
            }
            else {
                styleId = "mag7";
            }

            if ( colorIdx == 0 ) {
                styleId += QLatin1String(" blue");
            } else if ( colorIdx == 1 ) {
                styleId += QLatin1String(" bluewhite");
            } else if ( colorIdx == 3 ) {
                styleId += QLatin1String(" yellow");
            } else if ( colorIdx == 4 ) {
                styleId += QLatin1String(" orange");
            } else if ( colorIdx == 5 ) {
                styleId += QLatin1String(" red");
            } else if ( colorIdx == 6 ) {
                styleId += QLatin1String(" garnetred");
            } else {
                // white and no color ID
                styleId += QLatin1String(" white");
            }

            out << "   <Placemark> \n";

            QString name = line.mid( 7, 3 );
            if (name == QLatin1String("Alp")) {
                out << "        <name>" << QString::fromUtf8( "α" ) << "</name> \n";
            } else if (name == QLatin1String("Bet")) {
                out << "        <name>" << QString::fromUtf8( "β" ) << "</name> \n";
            } else if (name == QLatin1String("Gam")) {
                out << "        <name>" << QString::fromUtf8( "γ" ) << "</name> \n";
            } else if (name == QLatin1String("Del")) {
                out << "        <name>" << QString::fromUtf8( "δ" ) << "</name> \n";
            } else if (name == QLatin1String("Eps")) {
                out << "        <name>" << QString::fromUtf8( "ε" ) << "</name> \n";
            }

            out << "       <styleUrl>#" << styleId << "</styleUrl> \n"
                << "       <Point> \n"
                << "           <coordinates>" << longitude << "," << latitude << ",0" << "</coordinates> \n"
                << "       </Point> \n"
                << "   </Placemark> \n";

        } while ( !line.isNull() );
    }

    out << "</Document> \n"
        << "</kml> \n";

    file.close();
}

int main(int argc, char *argv[])
{
    QCoreApplication  app(argc, argv);

    if (!QFileInfo("catalog.dat").exists())
    {
        std::cerr << "Missing stars.dat in current directory. Exiting." << std::endl;
        return 1;
    }

    colorTable.append(double(-0.23)); // Spica blue
    colorTable.append(double(0.0)); //Rigel blue-white
    colorTable.append(double(0.09)); //Deneb white
    colorTable.append(double(0.80)); //Capella yellow
    colorTable.append(double(1.23)); //Arcturus orange
    colorTable.append(double(1.85)); //Betelgeuse red
    colorTable.append(double(2.35)); //Mu Cep garnet red

    exportToDat();
    exportToKml();

    QFile starFile("stars.dat");
    starFile.open(QIODevice::ReadOnly);
    QDataStream in(&starFile);    // read the data serialized from the file
 // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != 0x73746172)
        return -1;

    // Read the version
    qint32 version;
    in >> version;
    if (version > 004) {
        qDebug() << "stars.dat: file too new.";
     return -1;
    }
    int id;
    double ra;
    double de;
    double mag;
    int colorIdx;

    while ( !in.atEnd() ) {
        in >> id;
        in >> ra;
        in >> de;
        in >> mag;
        in >> colorIdx;
        qDebug() << "ID:" << id << "RA:" << ra << "DE:" << de << "MAG:" << mag << "idx:" << colorIdx;
    }

    app.exit();
}


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


#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QStringList>

#include <cmath>

#define ENABLEGUI

void exportToDat()
{
    // Set up Color Table Per B-V Color indices from some Reference Stars
    QVector<double> colorTable( 0 );

    colorTable.append(double(-0.23)); // Spica blue
    colorTable.append(double(0.0)); //Rigel blue-white
    colorTable.append(double(0.09)); //Deneb white
    colorTable.append(double(0.80)); //Capella yellow
    colorTable.append(double(1.23)); //Arcturus orange
    colorTable.append(double(1.85)); //Betelgeuse red
    colorTable.append(double(2.35)); //Mu Cep garnet red


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

            double deSign = ( decString.mid( 0, 1 ) == "-" ) ? -1.0 : 1.0;
            double deHH = decString.mid( 1, 2 ).toDouble();
            double deMM = decString.mid( 3, 2 ).toDouble();
            double deSS = decString.mid( 5, 2 ).toDouble();

            double deValue = deSign * ( deHH + deMM / 60.0 + deSS / 3600.0 ) / 180.0 * M_PI;

            QString magString = line.mid( 102, 5 );
            double magValue = magString.toDouble();

            QString bvString = line.mid( 108, 6);
            int     colorIdx = 2; // Default White

            // Find Index of Table Entry with Closest B-V value (Smallest Difference)
            if(bvString != QString("      ")) {
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
        << "<kml xmlns=\"http://earth.google.com/kml/2.2\" hint=\"target=sky\"> \n"
        << "<Document> \n"
        << "   <Style id=\"mag-1\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_-1_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag0\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_0_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag1\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_1_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag2\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_2_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag3\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_3_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag4\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_4_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag5\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_5_white.png</href> \n"
        << "           </Icon> \n"
        << "       </IconStyle> \n"
        << "   </Style> \n"
        << "   <Style id=\"mag6\"> \n"
        << "       <IconStyle> \n"
        << "           <Icon> \n"
        << "               <href>star_6_white.png</href> \n"
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
            double deSign = ( decString.mid( 0, 1 ) == "-" ) ? -1.0 : 1.0;
            double deHH = decString.mid( 1, 2 ).toDouble();
            double deMM = decString.mid( 3, 2 ).toDouble();
            double deSS = decString.mid( 5, 2 ).toDouble();

            double deValue = deSign * ( deHH + deMM / 60.0 + deSS / 3600.0 );

            qreal latitude = deValue;

            QString magString = line.mid( 102, 5 );
            double magValue = magString.toDouble();

            QString styleId;
            if ( magValue < -1 ) {
                styleId = "mag-1";
            }
            else if ( magValue < 0 ) {
                styleId = "mag0";
            }
            else if ( magValue < 1 ) {
                styleId = "mag1";
            }
            else if ( magValue < 2 ) {
                styleId = "mag2";
            }
            else if ( magValue < 3 ) {
                styleId = "mag3";
            }
            else if ( magValue < 4 ) {
                styleId = "mag4";
            }
            else if ( magValue < 5 ) {
                styleId = "mag5";
            }
            else if ( magValue < 6 ) {
                styleId = "mag6";
            }
            else {
                styleId = "mag6";
            }

            out << "   <Placemark> \n";

            QString name = line.mid( 7, 3 );
            if ( name == "Alp" ) {
                out << "        <name>" << QString::fromUtf8( "α" ) << "</name> \n";
            } else if ( name == "Bet" ) {
                out << "        <name>" << QString::fromUtf8( "β" ) << "</name> \n";
            } else if ( name == "Gam" ) {
                out << "        <name>" << QString::fromUtf8( "γ" ) << "</name> \n";
            } else if ( name == "Del" ) {
                out << "        <name>" << QString::fromUtf8( "δ" ) << "</name> \n";
            } else if ( name == "Eps" ) {
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

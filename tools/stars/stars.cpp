
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QStringList>

#include <cmath>

#define ENABLEGUI

class SimpleRgb
{
public:
    SimpleRgb(double d_bv=0.09, unsigned char d_red=0xff, unsigned char d_green=0xff, unsigned char d_blue=0xff) :
        bv(d_bv), red(0x000000ff&d_red), green(0x000000ff&d_green), blue(0x000000ff&d_blue)
        {};
    SimpleRgb(const SimpleRgb &rgb) {
        bv = rgb.bv;
        red = rgb.red;
        green = rgb.green;
        blue = rgb.blue;
    };

    double bv;
    int red;
    int green;
    int blue;
};

int main(int argc, char *argv[])
{
    QCoreApplication  app(argc, argv);

    // Set up Color Table Per B-V Color indices from some Reference Stars
    QVector<SimpleRgb> colorTable( 0 );

    colorTable.append(SimpleRgb(-1000.0, 0xff, 0xff, 0xff)); //Default White
    colorTable.append(SimpleRgb(-0.23, 0xa0, 0xb8, 0xf9)); // Spica
    colorTable.append(SimpleRgb(0.0, 0xb9, 0xcb, 0xf9)); //Rigel
    colorTable.append(SimpleRgb(0.09, 0xff, 0xff, 0xff)); //Deneb
    colorTable.append(SimpleRgb(0.80, 0xf8, 0xf9, 0xcc)); //Capella
    colorTable.append(SimpleRgb(1.23, 0xf8, 0xf0, 0xcc)); //Arcturus
    colorTable.append(SimpleRgb(1.85, 0xff, 0xe5, 0xd3)); //Betelgeuse
    colorTable.append(SimpleRgb(2.35, 0xe8, 0xbc, 0x95)); //Mu Cep


    QFile file("stars.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    // Write a header with a "magic number" and a version
    out << (quint32)0x73746172;
    out << (qint32)003;

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
            int     colorIdx = 0; // Default White

            // Find Index of Table Entry with Closest B-V value (Smallest Difference)
            if(bvString != QString("      ")) {
                double bvValue = bvString.toDouble();
                double bvMinDifference = fabs(colorTable.at(0).bv-bvValue);
                for (int i = 1; i < colorTable.size(); ++i) {
                    double bvDifference = fabs(colorTable.at(i).bv-bvValue);
                    if (bvDifference < bvMinDifference) {
                        colorIdx = i;
                        bvMinDifference = bvDifference;
                    }
                }
            }

            SimpleRgb rgb = colorTable.at(colorIdx);

//            qDebug() << "Rec:" << recString << "Dec.:" << decString << "Mag.:" << magString;
            if ( !line.isNull() && magValue < 6.0 ) {
                qDebug() << "ID:" << idValue << "RA:" << raValue << "DE:" << deValue << "mag:" << magValue << "B-V:" << bvString << "rgb:" << rgb.red << rgb.green << rgb.blue;
                out << idValue;
                out << raValue;
                out << deValue;
                out << magValue;
                out << rgb.red;
                out << rgb.green;
                out << rgb.blue;
            }
        } while ( !line.isNull() );
    }
    file.flush();

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
    if (version > 003) {
        qDebug() << "stars.dat: file too new.";
     return -1;
    }
    int id;
    double ra;
    double de;
    double mag;
    int red;
    int green;
    int blue;

    while ( !in.atEnd() ) {
        in >> id;
        in >> ra;
        in >> de;
        in >> mag;
        in >> red;
        in >> green;
        in >> blue;
        qDebug() << "ID:" << id << "RA:" << ra << "DE:" << de << "MAG:" << mag << "rgb:" << red << green << blue;
    }

    app.exit();
}

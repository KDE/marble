// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#include <cmath>

#include "Quaternion.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QStringList>

using namespace Marble;

int getHeader(int count, int size)
{
    int header = 0;
    if (size > 14) {
        if (count % 9 == 0)
            header = 5;
        else if (count % 5 == 0)
            header = 3;
        else if (count % 2 == 0)
            header = 2;
        else
            header = 1;
    } else if (size > 6) {
        if (count % 2 == 0)
            header = 3;
        else
            header = 1;
    } else {
        header = 2;
    }
    if (count == size - 1)
        header = 5;

    return header;
}

double deg2rad = M_PI / 180.0;

int main(int argc, char *argv[])
{
    QString sourcefilename;
    QString targetfilename;

    QCoreApplication app(argc, argv);

    if (argc != 4 || strcmp(argv[1], "-o") != 0) {
        qDebug(" dateline -o targetfile sourcefile");
        return 0;
    }

    targetfilename = QString(argv[2]);
    sourcefilename = QString(argv[3]);

    QList<float> idlPosList;

    QFile sourcefile(sourcefilename);
    sourcefile.open(QIODevice::ReadOnly);

    qDebug() << "Source: " << sourcefilename;
    qDebug() << "Target: " << targetfilename;

    // Read the data serialized from the file.
    QTextStream sourcestream(&sourcefile);

    QString rawline;
    QString lonstring;
    QString latstring;

    QStringList splitline;

    int line = 0;

    while (!sourcestream.atEnd()) {
        rawline = sourcestream.readLine();

        if (!rawline.contains(QLatin1Char(' '))) {
            qDebug() << "Line " << line << " does not contain a space separator.";
            continue;
        }
        splitline = rawline.split(QLatin1Char(' '));

        lonstring = splitline[0];
        latstring = splitline[1];

        //        qDebug() << "Point read at: " << lonstring << ", " << latstring;

        float lon = lonstring.toFloat();
        float lat = latstring.toFloat();

        idlPosList.append(lon);
        idlPosList.append(lat);
        ++line;
    }

    QFile targetfile(targetfilename);

    // Read the data serialized from the file.
    targetfile.open(QIODevice::WriteOnly);
    QDataStream stream(&targetfile);
    stream.setByteOrder(QDataStream::LittleEndian);

    int count = 0;
    bool firstheader = true;

    QList<float>::iterator i = idlPosList.begin();

    float lastlon = 99999.0f;
    float lastlat = 99999.0f;

    const float step = 50.0f;

    while (i != idlPosList.end()) {
        float lonf = *i++;
        float latf = *i++;
        //       qDebug() << "Writing point" << lonf << ", " << latf;

        float header;
        float lat;
        float lon;

        header = 5;
        if (firstheader) {
            header = 19000;
            firstheader = false;
        }

        lon = (lonf * 60.0f);
        lat = (latf * 60.0f);

        if (lastlon != 99999.0f || lastlat != 99999.0f) {
            Quaternion lastPos = Quaternion::fromSpherical((lastlon / 60.0) * deg2rad, (lastlat / 60.0) * -deg2rad);
            Quaternion currentPos = Quaternion::fromSpherical((lon / 60.0) * deg2rad, (lat / 60.0) * -deg2rad);
            //	    qDebug() << "lastPos: " << lastPos << "currentPos: " << currentPos;

            float distance = sqrt((lon - lastlon) * (lon - lastlon) + (lat - lastlat) * (lat - lastlat));
            int numsteps = (int)(distance / step);

            for (int i = 1; i < numsteps; ++i) {
                const Quaternion itPos = Quaternion::slerp(lastPos, currentPos, (double)(i) / (double)(numsteps));
                //		qDebug() << "itPos: " << itPos;
                double alpha = 0;
                double beta = 0;
                itPos.getSpherical(alpha, beta);
                //		qDebug() << "alpha: " << alpha << " beta: " << beta;
                float ipLon = alpha * 180.0 / M_PI * 60;
                float ipLat = -beta * 180.0 / M_PI * 60;

                short ipHeader = getHeader(i, numsteps);

                qDebug() << "numsteps: " << numsteps << "ipLon:" << (short)ipLon << "ipLat:" << (short)ipLat << "ipHeader:" << (short)ipHeader
                         << "node#:" << count;
                stream << (short)(ipHeader) << (short)(ipLat) << (short)(ipLon);

                count++;
            }
        }

        qDebug() << "lng:" << (short)(lon) << "lat:" << (short)(lat) << "header:" << (short)(header) << "node#:" << count;

        stream << (short)(header) << (short)(lat) << (short)(lon);

        lastlon = lon;
        lastlat = lat;
        count++;
    }
    targetfile.close();

    qDebug("Finished!");

    app.exit();
}

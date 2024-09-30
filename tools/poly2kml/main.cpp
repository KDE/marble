// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QTextStream>
#include <QTime>

#include "MarbleColors.h"
#include "geodata/data/GeoDataData.h"
#include "geodata/data/GeoDataDocument.h"
#include "geodata/data/GeoDataExtendedData.h"
#include "geodata/data/GeoDataLineString.h"
#include "geodata/data/GeoDataLineStyle.h"
#include "geodata/data/GeoDataLinearRing.h"
#include "geodata/data/GeoDataMultiGeometry.h"
#include "geodata/data/GeoDataPlacemark.h"
#include "geodata/data/GeoDataStyle.h"
#include "geodata/data/GeoDataStyleMap.h"
#include "geodata/handlers/kml/KmlElementDictionary.h"
#include "geodata/writer/GeoWriter.h"

using namespace Marble;

int usage()
{
    qDebug() << "Usage: [options] poly2kml input.poly output.kml";
    qDebug() << "\tOptions store additional metadata in output.kml:";
    qDebug() << "\t--version aVersion";
    qDebug() << "\t--name aName";
    qDebug() << "\t--date aDate";
    qDebug() << "\t--payload aFilename";
    return 1;
}

QColor randomColor()
{
    QVector<QColor> colors = QVector<QColor>() << Oxygen::aluminumGray4 << Oxygen::brickRed4;
    colors << Oxygen::hotOrange4 << Oxygen::forestGreen4 << Oxygen::hotOrange4;
    colors << Oxygen::seaBlue2 << Oxygen::skyBlue4 << Oxygen::sunYellow6;
    return colors.at(QRandomGenerator::global()->generate() % colors.size());
}

void parseBoundingBox(const QFileInfo &file,
                      const QString &name,
                      const QString &version,
                      const QString &date,
                      const QString &transport,
                      const QString &payload,
                      GeoDataDocument *document)
{
    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    GeoDataMultiGeometry *geometry = new GeoDataMultiGeometry;
    QFile input(file.absoluteFilePath());
    QString country = "Unknown";
    if (input.open(QFile::ReadOnly)) {
        QTextStream stream(&input);
        country = stream.readLine();
        float lat(0.0), lon(0.0);
        GeoDataLinearRing *box = new GeoDataLinearRing;
        while (!stream.atEnd()) {
            bool inside = true;
            QString line = stream.readLine().trimmed();
            QStringList entries = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (entries.size() == 1) {
                if (entries.first() == QLatin1StringView("END") && inside) {
                    inside = false;
                    if (!box->isEmpty()) {
                        geometry->append(box);
                        box = new GeoDataLinearRing;
                    }
                } else if (entries.first() == QLatin1StringView("END") && !inside) {
                    qDebug() << "END not expected here";
                } else if (entries.first().startsWith(QLatin1StringView("!"))) {
                    qDebug() << "Warning: Negative polygons not supported, skipping";
                } else {
                    // int number = entries.first().toInt();
                    inside = true;
                }
            } else if (entries.size() == 2) {
                lon = entries.first().toDouble();
                lat = entries.last().toDouble();
                GeoDataCoordinates point(lon, lat, 0.0, GeoDataCoordinates::Degree);
                *box << point;
            } else {
                qDebug() << "Warning: Ignoring line in" << file.absoluteFilePath() << "with" << entries.size() << "fields:" << line;
            }
        }
    }

    GeoDataStyle::Ptr style(new GeoDataStyle);
    GeoDataLineStyle lineStyle;
    QColor color = randomColor();
    color.setAlpha(200);
    lineStyle.setColor(color);
    lineStyle.setWidth(4);
    style->setLineStyle(lineStyle);
    style->setId("border");

    GeoDataStyleMap styleMap;
    styleMap.setId("map-border");
    styleMap.insert("normal", QLatin1Char('#') + style->id());
    document->addStyleMap(styleMap);
    document->addStyle(style);

    placemark->setStyleUrl(QLatin1Char('#') + styleMap.id());

    placemark->setName(name);
    if (!version.isEmpty()) {
        placemark->extendedData().addValue(GeoDataData("version", version));
    }
    if (!date.isEmpty()) {
        placemark->extendedData().addValue(GeoDataData("date", date));
    }
    if (!transport.isEmpty()) {
        placemark->extendedData().addValue(GeoDataData("transport", transport));
    }
    if (!payload.isEmpty()) {
        placemark->extendedData().addValue(GeoDataData("payload", payload));
    }
    placemark->setGeometry(geometry);
    document->append(placemark);
}

int save(GeoDataDocument *document, const QFileInfo &filename)
{
    GeoWriter writer;
    writer.setDocumentType(kml::kmlTag_nameSpaceOgc22);

    QFile file(filename.absoluteFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Cannot write to " << file.fileName();
        return usage();
    }

    if (!writer.write(&file, document)) {
        qDebug() << "Can not write to " << file.fileName();
    }
    file.close();
    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    if (argc < 3) {
        usage();
        return 0;
    }

    QString inputFile = argv[argc - 2];
    QString outputFile = argv[argc - 1];
    QString name;
    QString version;
    QString date;
    QString transport;
    QString payload;
    for (int i = 1; i < argc - 2; ++i) {
        QString arg(argv[i]);
        if (arg == QLatin1StringView("--name")) {
            name = argv[++i];
        } else if (arg == QLatin1StringView("--version")) {
            version = argv[++i];
        } else if (arg == QLatin1StringView("--date")) {
            date = argv[++i];
        } else if (arg == QLatin1StringView("--transport")) {
            transport = argv[++i];
        } else if (arg == QLatin1StringView("--payload")) {
            payload = argv[++i];
        } else {
            usage();
            return 1;
        }
    }

    QRandomGenerator::global()->seed(QTime::currentTime().msec());
    QFileInfo input(inputFile);
    if (!input.exists() || !input.isFile()) {
        qDebug() << "Invalid input file";
        return usage();
    }

    GeoDataDocument document;
    parseBoundingBox(input, name, version, date, transport, payload, &document);
    return save(&document, QFileInfo(outputFile));
}

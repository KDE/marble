// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>

#include "GpsbabelRunner.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "MarbleDebug.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QProcess>
#include <QTemporaryFile>

namespace Marble
{

GpsbabelRunner::GpsbabelRunner(QObject *parent)
    : ParsingRunner(parent)
{
}

GeoDataDocument *GpsbabelRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    // Check and see if the file exists
    if (!QFileInfo::exists(fileName)) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Inspect the filename suffix
    QString const fileSuffix = QFileInfo(fileName).suffix();

    // Determine if fileName suffix is supported by this plugin
    QMap<QString, QString> fileTypes;
    fileTypes[QStringLiteral("nmea")] = QStringLiteral("nmea");
    fileTypes[QStringLiteral("igc")] = QStringLiteral("igc");
    fileTypes[QStringLiteral("tiger")] = QStringLiteral("tiger");
    fileTypes[QStringLiteral("ov2")] = QStringLiteral("tomtom");
    fileTypes[QStringLiteral("garmin")] = QStringLiteral("garmin_txt");
    fileTypes[QStringLiteral("magellan")] = QStringLiteral("magellan");
    fileTypes[QStringLiteral("csv")] = QStringLiteral("csv");
    QString const inputFileType = fileTypes[fileSuffix];
    if (inputFileType.isEmpty()) {
        error = QStringLiteral("Unsupported file extension for").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Set up temporary file to hold output KML from gpsbabel executable
    QTemporaryFile tempKmlFile(QDir::tempPath() + QLatin1StringView("/marble-gpsbabel-XXXXXX.kml"));
    tempKmlFile.open();
    QFile kmlFile(tempKmlFile.fileName());

    // Set up gpsbabel command line
    const QString command = QLatin1StringView("gpsbabel");
    const QStringList args = QStringList() << QLatin1StringView("-i") << inputFileType << QLatin1StringView("-f") << fileName << QLatin1StringView("-o")
                                           << QLatin1StringView("kml") << QLatin1StringView("-F") << tempKmlFile.fileName();

    // Execute gpsbabel to parse the input file
    int const exitStatus = QProcess::execute(command, args);
    if (exitStatus == 0) {
        kmlFile.open(QIODevice::ReadWrite);
        GeoDataParser parser(GeoData_KML);
        parser.read(&kmlFile);
        auto document = dynamic_cast<GeoDataDocument *>(parser.releaseDocument());
        if (!document) {
            error = parser.errorString();
            mDebug() << error;
            return nullptr;
        }

        document->setDocumentRole(role);
        return document;
    } else {
        error = QStringLiteral("Gpsbabel returned error code %1").arg(exitStatus);
        mDebug() << error;
        return nullptr;
    }
}

}

#include "moc_GpsbabelRunner.cpp"

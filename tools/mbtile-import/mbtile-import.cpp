//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "MbTileWriter.h"

#include <QCoreApplication>
#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

using namespace std;
using namespace Marble;

void importTiles(const QString &tileDirectory, MbTileWriter &tileWriter, const QPair<int, int> &tileLevels)
{
    QString const extension = "o5m";
    QDir tileDir(tileDirectory);
    auto const strip = 1+tileDir.absolutePath().size();
    foreach(const auto &entryInfo, tileDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        bool isNumber;
        int const z = entryInfo.baseName().toInt(&isNumber);
        if (isNumber && tileLevels.first <= z && z <= tileLevels.second) {
            QDirIterator tileIter(entryInfo.absoluteFilePath(), QDirIterator::Subdirectories);
            for (; tileIter.hasNext(); tileIter.next()) {
                auto tileInfo = tileIter.fileInfo();
                if (!tileInfo.isFile() || tileInfo.completeSuffix() != extension) {
                    continue;
                }

                QString const tileId = tileInfo.absoluteFilePath().mid(strip);
                QStringList const tileEntries = tileId.split(QLatin1Char('/'));
                if (tileEntries.size() == 3) {
                    int const x = tileEntries[1].toInt(&isNumber);
                    if (isNumber && x >= 0) {
                        int const y = tileInfo.baseName().toInt(&isNumber);
                        if (isNumber && y >= 0) {
                            tileWriter.addTile(tileInfo, x, y, z);
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("mbtile-import");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Import tiles from a z/x/y.ext directory structure to a .mbtiles SQLite database.");
    auto const helpOption = parser.addHelpOption();
    auto const versionOption = parser.addVersionOption();
    parser.addPositionalArgument("directory", "Directory with tiles in z/x/y.ext structure");
    parser.addPositionalArgument("output", "Destination MBTile database");

    parser.addOptions({
                          {{"o", "overwrite"}, "Overwrite existing tiles in the database"},
                          {{"q", "quiet"}, "No progress report to stdout"},
                          {{"t", "tilelevels"}, "Restrict tile levels to <tilelevels>", "tilelevels", "0-20"},
                          {{"i", "interval"}, "Commit each <interval> tiles (0: single transaction)", "interval", "10000"},
                      });

    if (!parser.parse(QCoreApplication::arguments())) {
        qDebug() << parser.errorText();
        parser.showHelp(2);
    } else if (parser.isSet(helpOption)) {
        parser.showHelp(0);
    } else if (parser.isSet(versionOption)) {
        parser.showVersion();
        return 0;
    }

    const QStringList positionalArguments = parser.positionalArguments();
    if (positionalArguments.size() != 2) {
        parser.showHelp(positionalArguments.size() == 0 ? 0 : 1);
    }

    QString const tileDirectory = parser.positionalArguments()[0];
    if (!QFileInfo(tileDirectory).isDir()) {
        qDebug() << tileDirectory << "is not a directory";
        parser.showHelp(3);
    }

    QStringList const tileLevels = parser.value("tilelevels").split(QLatin1Char('-'));
    QPair<int, int> tileLevelRange = QPair<int, int>(0, 20);
    bool haveValidRange = false;
    if (tileLevels.size() == 2) {
        bool ok;
        tileLevelRange.first = tileLevels[0].toInt(&ok);
        if (ok) {
            tileLevelRange.second = tileLevels[1].toInt(&ok);
            if (ok) {
                haveValidRange = tileLevelRange.first >= 0 && tileLevelRange.first <= tileLevelRange.second && tileLevelRange.second <= 30;
            }
        }
    }

    if (!haveValidRange) {
        qDebug() << "Cannot parse tile level range. Expecting format 'minLevel-maxLevel', e.g. '3-7'.";
        return 4;
    }

    QString const mbTilesFile = parser.positionalArguments()[1];
    MbTileWriter tileWriter(mbTilesFile);
    tileWriter.setOverwriteTiles(parser.isSet("overwrite"));
    tileWriter.setReportProgress(!parser.isSet("quiet"));
    tileWriter.setCommitInterval(parser.value("interval").toInt());

    importTiles(tileDirectory, tileWriter, tileLevelRange);
    return 0;
}

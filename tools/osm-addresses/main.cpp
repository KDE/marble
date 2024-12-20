// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "SqlWriter.h"
#include "pbf/PbfParser.h"
#include "xml/XmlParser.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

#include <QMessageLogContext>

using namespace Marble;

enum DebugLevel {
    Debug,
    Info,
    Mute
};

DebugLevel debugLevel = Info;

void debugOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type) {
    case QtDebugMsg:
        if (debugLevel == Debug) {
            qDebug() << "Debug: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtInfoMsg:
    case QtWarningMsg:
        if (debugLevel < Mute) {
            qDebug() << "Info: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtCriticalMsg:
        if (debugLevel < Mute) {
            qDebug() << "Warning: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtFatalMsg:
        if (debugLevel < Mute) {
            qDebug() << "Fatal: " << context.file << ":" << context.line << " " << msg;
            abort();
        }
    }
}

void usage()
{
    qDebug() << "Usage: [options] osm-addresses [options] input.osm.pbf|input.osm output.sqlite output.kml";
    qDebug() << "\tOptions affect verbosity and store additional metadata in output.kml:";
    qDebug() << "\t-q quiet";
    qDebug() << "\t-v debug output";
    qDebug() << "\t--version aVersion";
    qDebug() << "\t--name aName";
    qDebug() << "\t--date aDate";
    qDebug() << "\t--payload aFilename";
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        usage();
        return 1;
    }

    QCoreApplication app(argc, argv);

    QString inputFile = argv[argc - 3];
    QString outputSqlite = argv[argc - 2];
    QString outputKml = argv[argc - 1];
    QString name;
    QString version;
    QString date;
    QString transport;
    QString payload;
    for (int i = 1; i < argc - 3; ++i) {
        QString arg(argv[i]);
        if (arg == QLatin1StringView("-v")) {
            debugLevel = Debug;
        } else if (arg == QLatin1StringView("-q")) {
            debugLevel = Mute;
        } else if (arg == QLatin1StringView("--name")) {
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

    qInstallMessageHandler(debugOutput);

    QFileInfo file(inputFile);
    if (!file.exists()) {
        qDebug() << "File " << file.absoluteFilePath() << " does not exist. Exiting.";
        return 2;
    }

    OsmParser *parser = nullptr;
    if (file.fileName().endsWith(QLatin1StringView(".osm"))) {
        parser = new XmlParser;
    } else if (file.fileName().endsWith(QLatin1StringView(".pbf"))) {
        parser = new PbfParser;
    } else {
        qDebug() << "Unsupported file format: " << file.fileName();
        return 3;
    }

    Q_ASSERT(parser);
    SqlWriter sql(outputSqlite);
    parser->addWriter(&sql);
    parser->read(file, name);
    parser->writeKml(name, version, date, transport, payload, outputKml);
}

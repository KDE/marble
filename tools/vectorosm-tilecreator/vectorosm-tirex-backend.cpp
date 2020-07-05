/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "tirexbackend.h"
#include "tirexmetatile.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    setenv("QT_LOGGING_TO_CONSOLE", "0", true); // redirects qDebug to syslog

    QCoreApplication app(argc, argv);
    TirexBackend backend;

    QObject::connect(&backend, &TirexBackend::tileRequested, &app, [&backend](const TirexMetatileRequest &req) {
        qDebug() << req.tile.x << req.tile.y << req.tile.z;
//         backend.tileDone(req);
        backend.tileError(req, QStringLiteral("not yet implemented"));
    });

    return app.exec();
}

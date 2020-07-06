/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "tirexbackend.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char **argv)
{
    setenv("QT_LOGGING_TO_CONSOLE", "0", true); // redirects qDebug to syslog

    QCoreApplication app(argc, argv);
    TirexBackend backend;

    QObject::connect(&backend, &TirexBackend::tileRequested, &app, [&backend](const TirexMetatileRequest &req) {
        qDebug() << req.tile.x << req.tile.y << req.tile.z << backend.metatileFileName(req);

        QFile f(backend.metatileFileName(req));
        if (!f.open(QFile::WriteOnly)) {
            backend.tileError(req, f.errorString());
            return;
        }

        backend.writeMetatileHeader(&f, req.tile);
        for (int x = 0; x < backend.metatileColumns(); ++x) {
            for (int y = 0; y < backend.metatileRows(); ++y) {
                QByteArray b = "TEST: " + QByteArray::number(x + req.tile.x) + "x" + QByteArray::number(y + req.tile.y);
                const auto offset = f.pos();
                f.write(b);
                backend.writeMetatileEntry(&f, x * backend.metatileColumns() + y, offset, b.size());
            }
        }
        backend.tileDone(req);
    });

    return app.exec();
}

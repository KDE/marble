//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "jobmanager.h"
#include "logger.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (app.arguments().size() < 3) {
        qDebug() << "Usage: " << app.arguments().at(0) << "regions.xml" << "log.sqlite";
        return 1;
    }

    Logger::instance().setFilename(app.arguments().at(2));

    JobManager manager;
    manager.setRegionsFile(app.arguments().at(1));
    manager.run();
    
    return app.exec();
}

// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#include "tccore.h"
#include <MarbleDebug.h>

using namespace Marble;

int main(int argc, char *argv[])
{
    TCCoreApplication app(argc, argv);
    MarbleDebug::setEnabled(true);
    if (argc < 2) {
        /*
                PREFIX: this is the prefix of the source directory
                TARGETDIR: the directory where the output should go to
                */
        qDebug() << "Syntax: tilecreator PREFIX TARGETDIR";
        return -1;
    } else {
        return app.exec();
    }
}

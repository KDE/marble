// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#include "tccore.h"

using namespace Marble;

TCCoreApplication::TCCoreApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    if (!(argc < 5)) {
        m_tilecreator = new TileCreator(argv[1], argv[2], argv[3], argv[4]);
        connect(m_tilecreator, &TileCreator::finished, this, &TCCoreApplication::quit);
        m_tilecreator->start();
    }
}

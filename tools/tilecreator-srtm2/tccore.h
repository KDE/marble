//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef TCCORE_H
#define TCCORE_H


#include <QCoreApplication>
#include <QDebug>

#include "TileCreator.h"

namespace Marble
{

class TCCoreApplication : public QCoreApplication
{
    public:
        TCCoreApplication( int argc, char ** argv );

    private:
        TileCreator *m_tilecreator;
};

}

#endif

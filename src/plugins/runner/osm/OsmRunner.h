//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEOSMRUNNER_H
#define MARBLEOSMRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class OsmRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit OsmRunner(QObject *parent = nullptr);
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;
};

}
#endif // MARBLEOSMRUNNER_H

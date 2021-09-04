//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEKMLRUNNER_H
#define MARBLEKMLRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class KmlRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit KmlRunner(QObject *parent = nullptr);
    ~KmlRunner() override;
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;
};

}
#endif // MARBLEKMLRUNNER_H

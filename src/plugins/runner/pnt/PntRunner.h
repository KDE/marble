//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEPNTRUNNER_H
#define MARBLEPNTRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class PntRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit PntRunner(QObject *parent = nullptr);
    ~PntRunner() override;
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;
};

}
#endif // MARBLEPNTRUNNER_H

//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLECACHERUNNER_H
#define MARBLECACHERUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class CacheRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit CacheRunner(QObject *parent = nullptr);
    ~CacheRunner() override;
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;

};

}
#endif // MARBLECACHERUNNER_H

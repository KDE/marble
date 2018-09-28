//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_LOG_RUNNER_H
#define MARBLE_LOG_RUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class LogRunner : public ParsingRunner
{
    Q_OBJECT

public:
    explicit LogRunner( QObject *parent = nullptr );
    ~LogRunner() override;

    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;
};

}

#endif // MARBLE_LOGFILE_RUNNER_H

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_LOGFILE_RUNNER_H
#define MARBLE_LOGFILE_RUNNER_H

#include "MarbleAbstractRunner.h"

namespace Marble
{

class LogRunner : public MarbleAbstractRunner
{
    Q_OBJECT

public:
    explicit LogRunner( QObject *parent = 0 );
    ~LogRunner();

    virtual void parseFile( const QString &fileName, DocumentRole role );
};

}

#endif // MARBLE_LOGFILE_RUNNER_H

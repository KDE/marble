//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLECACHERUNNER_H
#define MARBLECACHERUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class CacheRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit CacheRunner(QObject *parent = 0);
    ~CacheRunner();
    virtual void parseFile( const QString &fileName, DocumentRole role );

};

}
#endif // MARBLECACHERUNNER_H

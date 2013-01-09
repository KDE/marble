//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEGPXRUNNER_H
#define MARBLEGPXRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class GpxRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit GpxRunner(QObject *parent = 0);
    ~GpxRunner();
    virtual void parseFile( const QString &fileName, DocumentRole role );

};

}
#endif // MARBLEGPXRUNNER_H

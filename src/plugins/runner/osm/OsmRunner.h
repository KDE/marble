//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEOSMRUNNER_H
#define MARBLEOSMRUNNER_H

#include "MarbleAbstractRunner.h"

namespace Marble
{

class OsmRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit OsmRunner(QObject *parent = 0);
    ~OsmRunner();
    virtual void parseFile( const QString &fileName, DocumentRole role );
};

}
#endif // MARBLEOSMRUNNER_H

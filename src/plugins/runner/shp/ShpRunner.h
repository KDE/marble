//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLESHPRUNNER_H
#define MARBLESHPRUNNER_H

#include "MarbleAbstractRunner.h"

namespace Marble
{

class ShpRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit ShpRunner(QObject *parent = 0);
    ~ShpRunner();
    virtual void parseFile( const QString &fileName, DocumentRole role );
};

}
#endif // MARBLESHPRUNNER_H

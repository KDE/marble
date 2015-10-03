//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEKMLRUNNER_H
#define MARBLEKMLRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class KmlRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit KmlRunner(QObject *parent = 0);
    ~KmlRunner();
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error );
};

}
#endif // MARBLEKMLRUNNER_H

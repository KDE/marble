//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEPNTRUNNER_H
#define MARBLEPNTRUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class PntRunner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit PntRunner(QObject *parent = 0);
    ~PntRunner();
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error );
};

}
#endif // MARBLEPNTRUNNER_H

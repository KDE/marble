//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>

#ifndef MARBLE_GPSBABEL_RUNNER_H
#define MARBLE_GPSBABEL_RUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class GpsbabelRunner : public ParsingRunner
{
    Q_OBJECT

public:
    explicit GpsbabelRunner( QObject *parent = 0 );

    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error );

};

}

#endif // MARBLE_GPSBABEL_RUNNER_H

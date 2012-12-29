//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Cezar Mocan <mocancezar@gmail.com>

#ifndef MARBLEPN2RUNNER_H
#define MARBLEPN2RUNNER_H

#include "ParsingRunner.h"

namespace Marble
{

class GeoDataLineString;

class Pn2Runner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit Pn2Runner(QObject *parent = 0);
    ~Pn2Runner();
    bool errorCheckLat( qint16 lat );
    bool errorCheckLon( qint16 lon );
    bool importPolygon( QDataStream &stream, GeoDataLineString* linestring, quint32 nrAbsoluteNodes );
    virtual void parseFile( const QString &fileName, DocumentRole role );

signals:

public slots:

};

}
#endif // MARBLEPN2RUNNER_H

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

#include <QDataStream>

class QDataStream;

namespace Marble
{

class GeoDataLineString;

class Pn2Runner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit Pn2Runner(QObject *parent = 0);
    ~Pn2Runner();
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error );

private:
    static bool errorCheckLat( qint16 lat );
    static bool errorCheckLon( qint16 lon );
    static bool importPolygon( QDataStream &stream, GeoDataLineString* linestring, quint32 nrAbsoluteNodes );

    GeoDataDocument* parseForVersion1( const QString &fileName, DocumentRole role );
    GeoDataDocument* parseForVersion2( const QString &fileName, DocumentRole role );

    QDataStream m_stream;
    quint8 m_fileHeaderVersion;
    quint32 m_fileHeaderPolygons;
    bool m_isMapColorField;       // Whether the file contains color indexes
};

}
#endif // MARBLEPN2RUNNER_H

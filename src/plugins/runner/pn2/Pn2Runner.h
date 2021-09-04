// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Cezar Mocan <mocancezar@gmail.com>

#ifndef MARBLEPN2RUNNER_H
#define MARBLEPN2RUNNER_H

#include "ParsingRunner.h"

#include <QDataStream>


namespace Marble
{

class GeoDataLineString;

class Pn2Runner : public ParsingRunner
{
    Q_OBJECT
public:
    explicit Pn2Runner(QObject *parent = nullptr);
    ~Pn2Runner() override;
    GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) override;

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

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_WAYPOINTPARSER_H
#define MARBLE_WAYPOINTPARSER_H

#include "RoutingWaypoint.h"
#include "marble_export.h"

#include <QMap>
#include <QVariant>
#include <QStringList>

class QTextStream;

namespace Marble
{

class MARBLE_EXPORT WaypointParser
{
public:
    /** Fields which can be parsed */
    enum Field {
        Longitude,
        Latitude,
        JunctionType,
        RoadName,
        TotalSecondsRemaining,
        RoadType
    };

    /** Constructor */
    WaypointParser();

    /** Parses the given stream and returns the extracted waypoint list */
    RoutingWaypoints parse( QTextStream &stream ) const;

    /** Associate the zero-based field no index with the given semantic type */
    void setFieldIndex( Field field, int index );

    /** The line separator used in the stream passed to #parse. Default is "\n" */
    void setLineSeparator( const QString &separator );

    /** The field separator. Default is ',' */
    void setFieldSeparator( const QChar &separator );

    /** Associate the given string key with the given junction type */
    void addJunctionTypeMapping( const QString &key, RoutingWaypoint::JunctionType value );

private:
    template<class T>
    T readField( Field field, const QStringList &fields, const T &defaultValue = T() ) const {
        int index = m_fieldIndices[field];
        if ( index >= 0 && index < fields.size() ) {
            return QVariant( fields[index] ).value<T>();
        }

        return defaultValue;
    }

    QString m_lineSeparator;

    QChar m_fieldSeparator;

    QMap<Field, int> m_fieldIndices;

    QMap<QString, RoutingWaypoint::JunctionType> m_junctionTypeMapping;

    Q_DISABLE_COPY( WaypointParser )
};

} // namespace Marble

#endif // MARBLE_WAYPOINTPARSER_H

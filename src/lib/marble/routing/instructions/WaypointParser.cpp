//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "WaypointParser.h"

#include <QDebug>
#include <QStringList>

namespace Marble
{

// Template specialization to avoid shifting a QString through a QVariant and back
template<>
QString WaypointParser::readField<QString>( Field field, const QStringList &fields, const QString &defaultValue ) const {
    int index = m_fieldIndices[field];
    if ( index >= 0 && index < fields.size() ) {
        return fields[index];
    }

    return defaultValue;
}

// The default values are suitable for older versions of gosmore (the one shipped with Ubuntu Lucid Lynx)
WaypointParser::WaypointParser() : m_lineSeparator(QStringLiteral("\n")), m_fieldSeparator(QLatin1Char(','))
{
    setFieldIndex( Latitude, 0 );
    setFieldIndex( Longitude, 1 );
    setFieldIndex( JunctionType, 2 );
    setFieldIndex( RoadName, 4 );
}

void WaypointParser::setFieldIndex( Field field, int index )
{
    m_fieldIndices[field] = index;
}

void WaypointParser::setLineSeparator( const QString &separator )
{
    m_lineSeparator = separator;
}

void WaypointParser::setFieldSeparator( const QChar &separator )
{
    m_fieldSeparator = separator;
}

void WaypointParser::addJunctionTypeMapping( const QString &key, RoutingWaypoint::JunctionType value )
{
    m_junctionTypeMapping[key] = value;
}

RoutingWaypoints WaypointParser::parse( QTextStream &stream ) const
{
    RoutingWaypoints result;
    QString input = stream.readAll();
    QStringList lines = input.split( m_lineSeparator );
    foreach( const QString &line, lines ) {
        if ( !line.trimmed().isEmpty() &&
             !line.trimmed().startsWith(QLatin1Char('#')) &&
             !line.startsWith( QLatin1String( "Content-Type: text/plain" ) ) ) {
            QStringList entries = line.split( m_fieldSeparator );
            if ( entries.size() >= 1 + m_fieldIndices[RoadName] ) {
                qreal lon = readField<qreal>( Longitude, entries );
                qreal lat = readField<qreal>( Latitude, entries );
                RoutingPoint point( lon, lat );
                QString junctionTypeRaw = readField<QString>( JunctionType, entries, QString() );
                RoutingWaypoint::JunctionType junctionType = RoutingWaypoint::Other;
                if ( m_junctionTypeMapping.contains( junctionTypeRaw ) ) {
                  junctionType = m_junctionTypeMapping[junctionTypeRaw];
                }
                QString roadType = readField<QString>( RoadType, entries, QString() );
                int secondsRemaining = readField<int>( TotalSecondsRemaining, entries, -1 );
                QString roadName = readField<QString>( RoadName, entries, QString() );

                // Road names may contain the field separator
                for (int i = 2 + m_fieldIndices[RoadName]; i<entries.size(); ++i)
                {
                  roadName += m_fieldSeparator + entries.at(i);
                }

                RoutingWaypoint item( point, junctionType, junctionTypeRaw, roadType, secondsRemaining, roadName );
                result.push_back( item );
            } else {
                qDebug() << "Cannot parse " << line << "(detected " << entries.size() << " fields)";
            }
        }
    }

    return result;
}

} // namespace Marble

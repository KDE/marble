//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingModel.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"

#include <QtCore/QVector>
#include <QtCore/QTime>
#include <QtCore/QBuffer>
#include <QtCore/QRegExp>
#include <QtGui/QPixmap>
#include <QtXml/QDomDocument>

namespace Marble {

    struct RouteElement
    {
        GeoDataCoordinates position;
        RoutingModel::RoutingItemType type;
        QString description;
    };

    /** @todo: Eventually switch to GeoDataDocument as the underlying storage */
    typedef QVector<RouteElement> RouteElements;

    class RoutingModelPrivate
    {
    public:
        RouteElements m_route;

        QTime m_totalTime;

        qreal m_totalDistance;

        RoutingModelPrivate();

        RouteElement parseGmlPos( const QStringList &content ) const;
    };

    RoutingModelPrivate::RoutingModelPrivate() : m_totalDistance( 0.0 )
    {
      // nothing to do
    }

    RouteElement RoutingModelPrivate::parseGmlPos( const QStringList &content ) const
    {
        Q_ASSERT( content.length() == 2 );

        RouteElement element;
        GeoDataCoordinates position;
        position.setLongitude( content.at( 0 ).toDouble(), GeoDataCoordinates::Degree );
        position.setLatitude( content.at( 1 ).toDouble(), GeoDataCoordinates::Degree );
        element.position = position;

        return element;
    }

    RoutingModel::RoutingModel( QObject *parent ) :
            QAbstractListModel( parent ), d( new RoutingModelPrivate )
    {
        // nothing to do
    }

    RoutingModel::~RoutingModel()
    {
        delete d;
    }

    int RoutingModel::rowCount ( const QModelIndex & parent ) const
    {
        return parent.isValid() ? 0 : d->m_route.size();
    }

    QVariant RoutingModel::headerData ( int section, Qt::Orientation orientation, int role ) const
    {
        if ( orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0 )
        {
            return QString( "Instruction" );
        }

        return QAbstractListModel::headerData( section, orientation, role );
    }

    QVariant RoutingModel::data ( const QModelIndex & index, int role ) const
    {
        if ( !index.isValid() )
            return QVariant();

        if ( index.row() < d->m_route.size() && index.column() == 0 )
        {
            RouteElement element = d->m_route.at( index.row() );
            switch ( role ) {
            case Qt::DisplayRole:
            case Qt::ToolTipRole:
                return element.description;
                break;
            case Qt::DecorationRole:
                if ( element.type == Instruction )
                    return QPixmap( MarbleDirs::path( "bitmaps/routing_step.png" ) );

                return QVariant();
                break;
            case CoordinateRole:
                return QVariant::fromValue( d->m_route.at( index.row() ).position );
                break;
            case TypeRole:
                return QVariant::fromValue( d->m_route.at( index.row() ).type );
                break;
            default:
                return QVariant();
            }
        }

        return QVariant();
    }

    void RoutingModel::importOpenGis( const QByteArray &content )
    {
        d->m_route.clear();

        QDomDocument xml;
        if ( !xml.setContent( content ) ) {
            qWarning() << "Cannot parse xml file with routing instructions.";
            return;
        }

        QDomElement root = xml.documentElement();

        QDomNodeList summary = root.elementsByTagName( "xls:RouteSummary" );
        if ( summary.size() > 0 ) {
            QDomNodeList time = summary.item( 0 ).toElement().elementsByTagName( "xls:TotalTime" );
            QDomNodeList distance = summary.item( 0 ).toElement().elementsByTagName( "xls:TotalDistance" );
            if ( time.size() == 1 && distance.size() == 1 ) {
              QRegExp regexp = QRegExp( "^PT(?:(\\d+)H)?(?:(\\d+)M)?(\\d+)S" );
              if ( regexp.indexIn( time.item( 0 ).toElement().text() ) == 0 ) {
                QStringList matches = regexp.capturedTexts();
                int hours( 0 ), minutes( 0 ), seconds( 0 );
                switch( matches.size() ) {
                case 4:
                  hours   = regexp.cap( matches.size()-3 ).toInt();
                  // Intentionally no break
                case 3:
                  minutes = regexp.cap( matches.size()-2 ).toInt();
                  // Intentionally no break
                case 2:
                  seconds = regexp.cap( matches.size()-1 ).toInt();
                  break;
                default:
                  qWarning() << "Unable to parse time string " << time.item( 0 ).toElement().text();
                }

                d->m_totalTime = QTime( hours, minutes, seconds, 0 );
                d->m_totalDistance = distance.item( 0 ).attributes().namedItem( "value" ).nodeValue().toDouble();
                QString unit = distance.item( 0 ).attributes().namedItem( "uom" ).nodeValue();
                if ( unit == "M" ) {
                  d->m_totalDistance *= METER2KM;
                }
                else if ( unit != "KM" ) {
                  qWarning() << "Cannot parse distance unit " << unit << ", treated as km.";
                }
              }
            }
        }

        QDomNodeList geometry = root.elementsByTagName( "xls:RouteGeometry" );
        if ( geometry.size() > 0 ) {
            QDomNodeList waypoints = geometry.item( 0 ).toElement().elementsByTagName( "gml:pos" );
            for( unsigned int i = 0; i < waypoints.length(); ++i )
            {
                QDomNode node = waypoints.item( i );
                QStringList content = node.toElement().text().split( ' ' );
                if ( content.length() == 2 ) {
                    RouteElement element = d->parseGmlPos( content );
                    element.type = WayPoint;
                    d->m_route.push_back( element );
                }
            }
        }

        QDomNodeList instructionList = root.elementsByTagName( "xls:RouteInstructionsList" );
        if ( instructionList.size() > 0 ) {
            QDomNodeList instructions = instructionList.item( 0 ).toElement().elementsByTagName( "xls:RouteInstruction" );
            for( unsigned int i = 0; i < instructions.length(); ++i )
            {               
                QDomElement node = instructions.item( i ).toElement();

                QDomNodeList textNodes = node.elementsByTagName( "xls:Instruction" );
                QDomNodeList positions = node.elementsByTagName( "gml:pos" );

                if ( textNodes.size()>0 && positions.size()>0 ) {
                    QStringList content = positions.at( 0 ).toElement().text().split( ' ' );
                    if ( content.length() == 2 ) {
                        RouteElement element = d->parseGmlPos( content );
                        element.description = textNodes.item( 0 ).toElement().text();
                        element.type = Instruction;
                        d->m_route.push_back( element );
                    }
                }
            }
        }

        reset();
    }

    QTime RoutingModel::totalTime() const
    {
      return d->m_totalTime;
    }

    qreal RoutingModel::totalDistance() const
    {
      return d->m_totalDistance;
    }

void RoutingModel::exportGpx( QIODevice *device ) const
{
    QString content( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n" );
    content += "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"Marble\" version=\"1.1\" ";
    content += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    content += "xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\n";
    content += "<metadata><link href=\"http://edu.kde.org/marble\"><text>Marble Virtual Globe</text></link></metadata>\n";

    QList<RouteElement> instructions;
    content += "<trk>\n<name>Route</name>\n<trkseg>\n";
    foreach( const RouteElement &element, d->m_route ) {
        if ( element.type == WayPoint ) {
            qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
            qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
            content += QString( "<trkpt lat=\"%1\" lon=\"%2\"></trkpt>\n" ).arg( lat,0,'f',7 ).arg( lon,0,'f',7 );
        }
        else {
          instructions << element;
        }
    }
    content += "</trkseg></trk>";

    content += "<rte>\n<name>Route</name>\n";
    foreach( const RouteElement &element, instructions ) {
        Q_ASSERT( element.type == Instruction );
        qreal lon = element.position.longitude( GeoDataCoordinates::Degree );
        qreal lat = element.position.latitude( GeoDataCoordinates::Degree );
        content += QString( "<rtept lat=\"%1\" lon=\"%2\">%3</rtept>\n" ).arg( lat,0,'f',7 ).arg( lon,0,'f',7 ).arg( element.description );
    }

    content += "</rte>\n";
    content += "</gpx>\n";

    device->write( content.toLocal8Bit() );
}

} // namespace Marble

#include "RoutingModel.moc"

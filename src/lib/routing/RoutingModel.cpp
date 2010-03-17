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
#include "gps/GpxFile.h"
#include "gps/RouteContainer.h"
#include "gps/WaypointContainer.h"
#include "gps/Waypoint.h"
#include "gps/Route.h"
#include "gps/TrackPoint.h"
#include "GeoDataCoordinates.h"
#include "FileLoader.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"

#include <QtCore/QVector>
#include <QtCore/QBuffer>
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

        RouteElement parseGmlPos(const QStringList &content) const;
    };

    RouteElement RoutingModelPrivate::parseGmlPos(const QStringList &content) const
    {
        Q_ASSERT(content.length() == 2);

        RouteElement element;
        GeoDataCoordinates position;
        position.setLongitude(content.at(0).toDouble(), GeoDataCoordinates::Degree);
        position.setLatitude(content.at(1).toDouble(), GeoDataCoordinates::Degree);
        element.position = position;

        return element;
    }

    RoutingModel::RoutingModel(QObject *parent) :
            QAbstractListModel(parent), d(new RoutingModelPrivate)
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
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
        {
            return QString("Instruction");
        }

        return QAbstractListModel::headerData(section, orientation, role);
    }

    QVariant RoutingModel::data ( const QModelIndex & index, int role ) const
    {
        if (!index.isValid())
            return QVariant();

        if (index.row() < d->m_route.size() && index.column() == 0)
        {
            RouteElement element = d->m_route.at(index.row());
            switch (role) {
            case Qt::DisplayRole:
            case Qt::ToolTipRole:
                return element.description;
                break;
            case Qt::DecorationRole:
                if (element.type == Instruction)
                    return QPixmap(MarbleDirs::path( "bitmaps/routing_step.png" ));

                return QVariant();
                break;
            case CoordinateRole:
                return QVariant::fromValue(d->m_route.at(index.row()).position);
                break;
            case TypeRole:
                return QVariant::fromValue(d->m_route.at(index.row()).type);
                break;
            default:
                return QVariant();
            }
        }

        return QVariant();
    }

    void RoutingModel::importOpenGis( const QByteArray &content)
    {
        d->m_route.clear();

        QDomDocument xml;
        if (!xml.setContent(content)) {
            qWarning() << "Cannot parse xml file with routing instructions.";
            return;
        }

        QDomElement root = xml.documentElement();

        QDomNodeList geometry = root.elementsByTagName("xls:RouteGeometry");
        if (geometry.size() > 0) {
            QDomNodeList waypoints = geometry.item(0).toElement().elementsByTagName("gml:pos");
            for(unsigned int i = 0; i < waypoints.length(); ++i )
            {
                QDomNode node = waypoints.item(i);
                QStringList content = node.toElement().text().split(' ');
                if (content.length() == 2) {
                    RouteElement element = d->parseGmlPos(content);
                    element.type = WayPoint;
                    d->m_route.push_back(element);
                }
            }
        }

        QDomNodeList instructionList = root.elementsByTagName("xls:RouteInstructionsList");
        if (instructionList.size() > 0) {
            QDomNodeList instructions = instructionList.item(0).toElement().elementsByTagName("xls:RouteInstruction");
            for(unsigned int i = 0; i < instructions.length(); ++i )
            {               
                QDomElement node = instructions.item(i).toElement();

                QDomNodeList textNodes = node.elementsByTagName("xls:Instruction");
                QDomNodeList positions = node.elementsByTagName("gml:pos");

                if (textNodes.size()>0 && positions.size()>0) {
                    QStringList content = positions.at(0).toElement().text().split(' ');
                    if (content.length() == 2) {
                        RouteElement element = d->parseGmlPos(content);
                        element.description = textNodes.item(0).toElement().text();
                        element.type = Instruction;
                        d->m_route.push_back(element);
                    }
                }
            }
        }

        reset();
    }

} // namespace Marble

#include "RoutingModel.moc"

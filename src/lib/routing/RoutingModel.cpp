/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

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

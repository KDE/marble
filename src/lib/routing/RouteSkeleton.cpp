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

#include "RouteSkeleton.h"

#include "GeoDataLineString.h"
#include "MarbleDirs.h"

#include <QtCore/QMap>
#include <QtGui/QPainter>

namespace Marble {

class RouteSkeletonPrivate
{
public:
    QVector<GeoDataCoordinates> m_route;

    QMap<int,QPixmap> m_pixmapCache;

    /** Determines a suitable index for inserting a via point */
    int viaIndex(const GeoDataCoordinates &position) const;
};

int RouteSkeletonPrivate::viaIndex(const GeoDataCoordinates &position) const
{
    /** @todo: Works, but does not look elegant at all */

    // Iterates over all ordered trip point pairs (P,Q) and finds the triple
    // (P,position,Q) or (P,Q,position) with minimum length
    qreal minLength = -1.0;
    int result = 0;
    GeoDataLineString viaFirst;
    GeoDataLineString viaSecond;
    for (int i=0; i<m_route.size(); ++i) {
        Q_ASSERT(viaFirst.size() < 4 && viaSecond.size() < 4);
        if (viaFirst.size() == 3) {
            viaFirst.remove(0);
            viaFirst.remove(0);
        }

        if (viaSecond.size() == 3) {
            viaSecond.remove(0);
            viaSecond.remove(0);
        }

        if (viaFirst.size() == 1) {
            viaFirst.append(position);
        }

        viaFirst.append(m_route[i]);
        viaSecond.append(m_route[i]);

        if (viaSecond.size() == 2) {
            viaSecond.append(position);
        }

        if (viaFirst.size() == 3) {
            qreal len = viaFirst.length(EARTH_RADIUS);
            if (minLength < 0.0 || len < minLength) {
                minLength = len;
                result = i;
            }
        }

        /** @todo: Assumes that destination is the last point */
        if (viaSecond.size() == 3 && i+1 < m_route.size() ) {
            qreal len = viaSecond.length(EARTH_RADIUS);
            if (minLength < 0.0 || len < minLength) {
                minLength = len;
                result = i+1;
            }
        }
    }

    Q_ASSERT( 0 <= result && result <= m_route.size() );
    return result;
}

RouteSkeleton::RouteSkeleton(QObject *parent) :
        QObject(parent), d(new RouteSkeletonPrivate)
{
    // nothing to do
}

RouteSkeleton::~RouteSkeleton()
{
    delete d;
}

int RouteSkeleton::size() const
{
    return d->m_route.size();
}

GeoDataCoordinates RouteSkeleton::source() const
{
    GeoDataCoordinates result;
    if (d->m_route.size())
        result = d->m_route.first();
    return result;
}

GeoDataCoordinates RouteSkeleton::destination() const
{
    GeoDataCoordinates result;
    if (d->m_route.size())
        result = d->m_route.last();
    return result;
}

GeoDataCoordinates RouteSkeleton::at(int position) const
{
    return d->m_route.at(position);
}

QPixmap RouteSkeleton::pixmap(int position) const
{
    if (d->m_pixmapCache.contains(position)) {
        return d->m_pixmapCache[position];
    }

    // Transparent background
    QImage result(16,16,QImage::Format_ARGB32_Premultiplied);
    result.fill(qRgba(0, 0, 0, 0));

    // Paint a green circle
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(Qt::black));
    painter.setBrush(QBrush(QColor::fromRgb(55,164,44)));  // green, oxygen palette
    painter.drawEllipse(1,1,13,13);
    painter.setBrush(QColor(Qt::black));

    // Paint a character denoting the position (0=A, 1=B, 2=C, ...)
    char text = char('A' + position);
    painter.drawText(2,2,12,12, Qt::AlignCenter, QString(text));

    d->m_pixmapCache.insert(position, QPixmap::fromImage(result));
    return pixmap(position);
}

void RouteSkeleton::clear()
{
    d->m_route.clear();
}

void RouteSkeleton::insert(int index, const GeoDataCoordinates &coordinates)
{
    d->m_route.insert(index, coordinates);
}

void RouteSkeleton::append(const GeoDataCoordinates &coordinates)
{
    d->m_route.append(coordinates);
}

void RouteSkeleton::remove(int index)
{
    d->m_route.remove(index);
}

void RouteSkeleton::addVia( const GeoDataCoordinates &position )
{
    int index = d->viaIndex(position);
    d->m_route.insert(index, position);
    emit positionAdded(index);
}

void RouteSkeleton::setPosition( int index, const GeoDataCoordinates &position)
{
    if (index >= 0 && index < d->m_route.size()) {
        d->m_route[index] = position;
        emit positionChanged(index, position);
    }
}

} // namespace Marble

#include "RouteSkeleton.moc"

// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

// Own
#include "GeoDataContainer.h"
#include "GeoDataContainer_p.h"

// Marble
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataGeometry.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataPlacemark.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataTour.h"
#include "MarbleDebug.h"

#include <QDataStream>

namespace Marble
{

GeoDataContainer::GeoDataContainer()
    : GeoDataFeature(new GeoDataContainerPrivate)
{
}

GeoDataContainer::GeoDataContainer(GeoDataContainerPrivate *priv)
    : GeoDataFeature(priv)
{
    Q_D(GeoDataContainer);
    d->setParent(this);
}

GeoDataContainer::GeoDataContainer(const GeoDataContainer &other, GeoDataContainerPrivate *priv)
    : GeoDataFeature(other, priv)
{
    Q_D(GeoDataContainer);
    d->setParent(this);
}

GeoDataContainer::GeoDataContainer(const GeoDataContainer &other)
    : GeoDataFeature(other, new GeoDataContainerPrivate(*other.d_func()))
{
    Q_D(GeoDataContainer);
    d->setParent(this);
}

GeoDataContainer::~GeoDataContainer() = default;

GeoDataContainer &GeoDataContainer::operator=(const GeoDataContainer &other)
{
    if (this != &other) {
        Q_D(GeoDataContainer);
        *d = *other.d_func();
    }

    return *this;
}

bool GeoDataContainer::equals(const GeoDataContainer &other) const
{
    if (!GeoDataFeature::equals(other)) {
        return false;
    }

    Q_D(const GeoDataContainer);
    const GeoDataContainerPrivate *const other_d = other.d_func();
    QList<GeoDataFeature *>::const_iterator thisBegin = d->m_vector.constBegin();
    QList<GeoDataFeature *>::const_iterator thisEnd = d->m_vector.constEnd();
    QList<GeoDataFeature *>::const_iterator otherBegin = other_d->m_vector.constBegin();
    QList<GeoDataFeature *>::const_iterator otherEnd = other_d->m_vector.constEnd();

    for (; thisBegin != thisEnd && otherBegin != otherEnd; ++thisBegin, ++otherBegin) {
        if (**thisBegin != **otherBegin) {
            return false;
        }
    }

    return thisBegin == thisEnd && otherBegin == otherEnd;
}

GeoDataLatLonAltBox GeoDataContainer::latLonAltBox() const
{
    Q_D(const GeoDataContainer);
    GeoDataLatLonAltBox result;

    QList<GeoDataFeature *>::const_iterator it = d->m_vector.constBegin();
    QList<GeoDataFeature *>::const_iterator end = d->m_vector.constEnd();
    for (; it != end; ++it) {
        // Get all the placemarks from GeoDataContainer
        if (const GeoDataPlacemark *placemark = geodata_cast<GeoDataPlacemark>(*it)) {
            // Only use visible placemarks for extracting their latLonAltBox and
            // making an union with the global latLonAltBox Marble will fit its
            // zoom to
            if (placemark->isVisible()) {
                if (result.isEmpty()) {
                    result = placemark->geometry()->latLonAltBox();
                } else {
                    result |= placemark->geometry()->latLonAltBox();
                }
            }
        } else if (const GeoDataContainer *container = dynamic_cast<GeoDataContainer *>(*it)) {
            if (result.isEmpty()) {
                result = container->latLonAltBox();
            } else {
                result |= container->latLonAltBox();
            }
        }
    }
    return result;
}

QList<GeoDataFolder *> GeoDataContainer::folderList() const
{
    Q_D(const GeoDataContainer);
    QList<GeoDataFolder *> results;

    QList<GeoDataFeature *>::const_iterator it = d->m_vector.constBegin();
    QList<GeoDataFeature *>::const_iterator end = d->m_vector.constEnd();

    for (; it != end; ++it) {
        auto folder = dynamic_cast<GeoDataFolder *>(*it);
        if (folder) {
            results.append(folder);
        }
    }

    return results;
}

QList<GeoDataPlacemark *> GeoDataContainer::placemarkList() const
{
    Q_D(const GeoDataContainer);
    QList<GeoDataPlacemark *> results;
    for (auto it = d->m_vector.constBegin(), end = d->m_vector.constEnd(); it != end; ++it) {
        if (auto placemark = geodata_cast<GeoDataPlacemark>(*it)) {
            results.append(placemark);
        }
    }
    return results;
}

QList<GeoDataFeature *> GeoDataContainer::featureList() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector;
}

/**
 * @brief  returns the requested child item
 */
GeoDataFeature *GeoDataContainer::child(int i)
{
    Q_D(GeoDataContainer);
    return d->m_vector.at(i);
}

const GeoDataFeature *GeoDataContainer::child(int i) const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.at(i);
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataContainer::childPosition(const GeoDataFeature *object) const
{
    Q_D(const GeoDataContainer);
    for (int i = 0; i < d->m_vector.size(); ++i) {
        if (d->m_vector.at(i) == object) {
            return i;
        }
    }
    return -1;
}

void GeoDataContainer::insert(GeoDataFeature *other, int index)
{
    insert(index, other);
}

void GeoDataContainer::insert(int index, GeoDataFeature *feature)
{
    Q_D(GeoDataContainer);
    feature->setParent(this);
    d->m_vector.insert(index, feature);
}

void GeoDataContainer::append(GeoDataFeature *other)
{
    Q_D(GeoDataContainer);
    other->setParent(this);
    d->m_vector.append(other);
}

void GeoDataContainer::remove(int index)
{
    Q_D(GeoDataContainer);
    d->m_vector.remove(index);
}

void GeoDataContainer::remove(int index, int count)
{
    Q_D(GeoDataContainer);
    d->m_vector.remove(index, count);
}

int GeoDataContainer::removeAll(GeoDataFeature *feature)
{
    Q_D(GeoDataContainer);
    return d->m_vector.removeAll(feature);
}

void GeoDataContainer::removeAt(int index)
{
    Q_D(GeoDataContainer);
    d->m_vector.removeAt(index);
}

void GeoDataContainer::removeFirst()
{
    Q_D(GeoDataContainer);
    d->m_vector.removeFirst();
}

void GeoDataContainer::removeLast()
{
    Q_D(GeoDataContainer);
    d->m_vector.removeLast();
}

bool GeoDataContainer::removeOne(GeoDataFeature *feature)
{
    Q_D(GeoDataContainer);
    return d->m_vector.removeOne(feature);
}

int GeoDataContainer::size() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.size();
}

bool GeoDataContainer::isEmpty() const
{
    return size() == 0;
}

GeoDataFeature &GeoDataContainer::at(int pos)
{
    Q_D(GeoDataContainer);
    return *(d->m_vector[pos]);
}

const GeoDataFeature &GeoDataContainer::at(int pos) const
{
    Q_D(const GeoDataContainer);
    return *(d->m_vector.at(pos));
}

GeoDataFeature &GeoDataContainer::last()
{
    Q_D(GeoDataContainer);
    return *(d->m_vector.last());
}

const GeoDataFeature &GeoDataContainer::last() const
{
    Q_D(const GeoDataContainer);
    return *(d->m_vector.last());
}

GeoDataFeature &GeoDataContainer::first()
{
    Q_D(GeoDataContainer);
    return *(d->m_vector.first());
}

const GeoDataFeature &GeoDataContainer::first() const
{
    Q_D(const GeoDataContainer);
    return *(d->m_vector.first());
}

void GeoDataContainer::clear()
{
    Q_D(GeoDataContainer);
    qDeleteAll(d->m_vector);
    d->m_vector.clear();
}

QList<GeoDataFeature *>::Iterator GeoDataContainer::begin()
{
    Q_D(GeoDataContainer);
    return d->m_vector.begin();
}

QList<GeoDataFeature *>::Iterator GeoDataContainer::end()
{
    Q_D(GeoDataContainer);
    return d->m_vector.end();
}

QList<GeoDataFeature *>::ConstIterator GeoDataContainer::constBegin() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.constBegin();
}

QList<GeoDataFeature *>::ConstIterator GeoDataContainer::constEnd() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.constEnd();
}

void GeoDataContainer::pack(QDataStream &stream) const
{
    Q_D(const GeoDataContainer);
    GeoDataFeature::pack(stream);

    stream << d->m_vector.count();

    for (QList<GeoDataFeature *>::const_iterator iterator = d->m_vector.constBegin(); iterator != d->m_vector.constEnd(); ++iterator) {
        const GeoDataFeature *feature = *iterator;
        stream << feature->featureId();
        feature->pack(stream);
    }
}

void GeoDataContainer::unpack(QDataStream &stream)
{
    Q_D(GeoDataContainer);
    GeoDataFeature::unpack(stream);

    int count;
    stream >> count;

    for (int i = 0; i < count; ++i) {
        int featureId;
        stream >> featureId;
        switch (featureId) {
        case GeoDataDocumentId:
            /* not usable!!!! */ break;
        case GeoDataFolderId: {
            auto folder = new GeoDataFolder;
            folder->unpack(stream);
            d->m_vector.append(folder);
        } break;
        case GeoDataPlacemarkId: {
            auto placemark = new GeoDataPlacemark;
            placemark->unpack(stream);
            d->m_vector.append(placemark);
        } break;
        case GeoDataNetworkLinkId:
            break;
        case GeoDataScreenOverlayId:
            break;
        case GeoDataGroundOverlayId:
            break;
        default:
            break;
        };
    }
}

}

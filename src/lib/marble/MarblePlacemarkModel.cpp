// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

// Own
#include "MarblePlacemarkModel.h"
#include "MarblePlacemarkModel_P.h"

// Qt
#include <QElapsedTimer>
#include <QImage>

// Marble
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataGeometry.h"
#include "GeoDataIconStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h" // In geodata/data/
#include "MarbleDebug.h"

using namespace Marble;

class Q_DECL_HIDDEN MarblePlacemarkModel::Private
{
public:
    Private()
        : m_size(0)
        , m_placemarkContainer(nullptr)
    {
    }

    ~Private() = default;

    int m_size;
    QList<GeoDataPlacemark *> *m_placemarkContainer;
};

// ---------------------------------------------------------------------------

MarblePlacemarkModel::MarblePlacemarkModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

MarblePlacemarkModel::~MarblePlacemarkModel()
{
    delete d;
}

void MarblePlacemarkModel::setPlacemarkContainer(QList<GeoDataPlacemark *> *container)
{
    d->m_placemarkContainer = container;
}

int MarblePlacemarkModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return d->m_size;
    else
        return 0;
}

int MarblePlacemarkModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 1;
    else
        return 0;
}

QHash<int, QByteArray> MarblePlacemarkModel::roleNames() const
{
    return {
        {DescriptionRole, "description"},
        {Qt::DisplayRole, "name"},
        {Qt::DecorationRole, "icon"},
        {IconPathRole, "iconPath"},
        {PopularityIndexRole, "zoomLevel"},
        {VisualCategoryRole, "visualCategory"},
        {AreaRole, "area"},
        {PopulationRole, "population"},
        {CountryCodeRole, "countryCode"},
        {StateRole, "state"},
        {PopularityRole, "popularity"},
        {GeoTypeRole, "role"},
        {CoordinateRole, "coordinate"},
        {StyleRole, "style"},
        {GmtRole, "gmt"},
        {DstRole, "dst"},
        {GeometryRole, "geometry"},
        {ObjectPointerRole, "objectPointer"},
        {LongitudeRole, "longitude"},
        {LatitudeRole, "latitude"},
    };
}

QVariant MarblePlacemarkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.row() >= d->m_placemarkContainer->size())
        return {};

    if (role == Qt::DisplayRole) {
        return d->m_placemarkContainer->at(index.row())->name();
    } else if (role == Qt::DecorationRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->style()->iconStyle().icon());
    } else if (role == IconPathRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->style()->iconStyle().iconPath());
    } else if (role == PopularityIndexRole) {
        return d->m_placemarkContainer->at(index.row())->zoomLevel();
    } else if (role == VisualCategoryRole) {
        return d->m_placemarkContainer->at(index.row())->visualCategory();
    } else if (role == AreaRole) {
        return d->m_placemarkContainer->at(index.row())->area();
    } else if (role == PopulationRole) {
        return d->m_placemarkContainer->at(index.row())->population();
    } else if (role == CountryCodeRole) {
        return d->m_placemarkContainer->at(index.row())->countryCode();
    } else if (role == StateRole) {
        return d->m_placemarkContainer->at(index.row())->state();
    } else if (role == PopularityRole) {
        return d->m_placemarkContainer->at(index.row())->popularity();
    } else if (role == DescriptionRole) {
        return d->m_placemarkContainer->at(index.row())->description();
    } else if (role == Qt::ToolTipRole) {
        return d->m_placemarkContainer->at(index.row())->description();
    } else if (role == GeoTypeRole) {
        return d->m_placemarkContainer->at(index.row())->role();
    } else if (role == CoordinateRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->coordinate());
    } else if (role == StyleRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->style().data());
    } else if (role == GmtRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->extendedData().value(QStringLiteral("gmt")).value());
    } else if (role == DstRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->extendedData().value(QStringLiteral("dst")).value());
    } else if (role == GeometryRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->geometry());
    } else if (role == ObjectPointerRole) {
        return QVariant::fromValue(dynamic_cast<GeoDataObject *>(d->m_placemarkContainer->at(index.row())));
    } else if (role == LongitudeRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->coordinate().longitude(GeoDataCoordinates::Degree));
    } else if (role == LatitudeRole) {
        return QVariant::fromValue(d->m_placemarkContainer->at(index.row())->coordinate().latitude(GeoDataCoordinates::Degree));
    } else
        return {};
}

QModelIndexList MarblePlacemarkModel::approxMatch(const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags) const
{
    QList<QModelIndex> results;

    int count = 0;

    QModelIndex entryIndex;
    QString listName;
    QString queryString = value.toString().toLower();
    QString simplifiedListName;

    int row = start.row();
    const int rowNum = rowCount();

    while (row < rowNum && count != hits) {
        if (flags & Qt::MatchStartsWith) {
            entryIndex = index(row, 0);
            listName = data(entryIndex, role).toString().toLower();
            simplifiedListName = GeoString::deaccent(listName);

            if (listName.startsWith(queryString) || simplifiedListName.startsWith(queryString)) {
                results << entryIndex;
                ++count;
            }
        }
        ++row;
    }

    return results;
}

void MarblePlacemarkModel::addPlacemarks(int start, int length)
{
    Q_UNUSED(start);

    // performance wise a reset is far better when the provided list
    // is significant. That is an issue because we have
    // MarbleControlBox::m_sortproxy as a sorting customer.
    // I leave the balance search as an exercise to the reader...

    QElapsedTimer t;
    t.start();
    //    beginInsertRows( QModelIndex(), start, start + length );
    d->m_size += length;
    //    endInsertRows();
    beginResetModel();
    endResetModel();
    Q_EMIT countChanged();
    mDebug() << "addPlacemarks: Time elapsed:" << t.elapsed() << "ms for" << length << "Placemarks.";
}

void MarblePlacemarkModel::removePlacemarks(const QString &containerName, int start, int length)
{
    if (length > 0) {
        QElapsedTimer t;
        t.start();
        beginRemoveRows(QModelIndex(), start, start + length);
        d->m_size -= length;
        endRemoveRows();
        Q_EMIT layoutChanged();
        Q_EMIT countChanged();
        mDebug() << "removePlacemarks(" << containerName << "): Time elapsed:" << t.elapsed() << "ms for" << length << "Placemarks.";
    }
}

#include "moc_MarblePlacemarkModel.cpp"

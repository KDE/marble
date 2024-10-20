// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

// Self
#include "CountryByShape.h"

// Qt
#include <QFileInfo>
#include <QList>
#include <QRandomGenerator>
#include <QTime>
#include <QVariant>
#include <QVariantList>

// Marble
#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleModel.h>
#include <marble/MarblePlacemarkModel.h>
#include <marble/MarbleWidget.h>

#include <marble/GeoDataDocument.h>
#include <marble/GeoDataGeometry.h>
#include <marble/GeoDataLatLonAltBox.h>
#include <marble/GeoDataLinearRing.h>
#include <marble/GeoDataMultiGeometry.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataPoint.h>
#include <marble/GeoDataPolygon.h>

namespace Marble
{
class CountryByShapePrivate
{
public:
    CountryByShapePrivate(MarbleWidget *marbleWidget)
        : m_parent(nullptr)
        , m_marbleWidget(marbleWidget)
        , m_countryNames(nullptr)
        , m_countryBoundaries(nullptr)
    {
        m_continentsAndOceans << QStringLiteral("Asia") << QStringLiteral("Africa") << QStringLiteral("North America") << QStringLiteral("South America")
                              << QStringLiteral("Antarctica") << QStringLiteral("Europe") << QStringLiteral("Australia") << QStringLiteral("Arctic Ocean")
                              << QStringLiteral("Indian Ocean") << QStringLiteral("North Atlantic Ocean") << QStringLiteral("North Pacific Ocean")
                              << QStringLiteral("South Pacific Ocean") << QStringLiteral("South Atlantic Ocean") << QStringLiteral("Southern Ocean");
    }

    CountryByShape *m_parent;
    MarbleWidget *m_marbleWidget;

    /**
     * Document to store point placemarks which
     * have country names ( from file "boundaryplacemarks.cache" )
     */
    GeoDataDocument *m_countryNames;

    /**
     * Document which have placemarks whose geometry
     * specifies the boundaries of a country
     * (from file "ne_50m_admin_0_countries.pn2" )
     */
    GeoDataDocument *m_countryBoundaries;

    /**
     * If the placemark, we select from boundaryplacemarks.cache,
     * is a continent, the game will highlight a country,
     * on map, which contains this point placemark in its geometry
     * instead of highlighting the whole continent.
     * Also, oceans are point placemark and we don't have
     * any geometry, provided for oceans , that we can highlight.
     * So, to avoid placemarks which represent continent or ocean
     * we will use this list.
     */
    QStringList m_continentsAndOceans;
};

CountryByShape::CountryByShape(MarbleWidget *marbleWidget)
    : QObject()
    , d(new CountryByShapePrivate(marbleWidget))
{
    d->m_parent = this;
    connect(this,
            SIGNAL(announceHighlight(qreal, qreal, GeoDataCoordinates::Unit)),
            d->m_marbleWidget,
            SIGNAL(highlightedPlacemarksChanged(qreal, qreal, GeoDataCoordinates::Unit)));
}

CountryByShape::~CountryByShape()
{
    delete d;
}

void CountryByShape::initiateGame()
{
    if (!d->m_countryNames) {
        const GeoDataTreeModel *const treeModel = d->m_marbleWidget->model()->treeModel();
        for (int i = 0; i < treeModel->rowCount(); ++i) {
            QVariant const data = treeModel->data(treeModel->index(i, 0), MarblePlacemarkModel::ObjectPointerRole);
            GeoDataObject *object = qvariant_cast<GeoDataObject *>(data);
            Q_ASSERT_X(object, "CountryByShape::initiateGame", "failed to get valid data from treeModel for GeoDataObject");
            if (auto doc = geodata_cast<GeoDataDocument>(object)) {
                QFileInfo fileInfo(doc->fileName());
                if (fileInfo.fileName() == QLatin1StringView("boundaryplacemarks.cache")) {
                    d->m_countryNames = doc;
                    break;
                }
            }
        }
    }

    if (!d->m_countryBoundaries) {
        const GeoDataTreeModel *const treeModel = d->m_marbleWidget->model()->treeModel();
        for (int i = 0; i < treeModel->rowCount(); ++i) {
            QVariant const data = treeModel->data(treeModel->index(i, 0), MarblePlacemarkModel::ObjectPointerRole);
            GeoDataObject *object = qvariant_cast<GeoDataObject *>(data);
            Q_ASSERT_X(object, "MainWindow::initiateGame", "failed to get valid data from treeModel for GeoDataObject");
            if (auto doc = geodata_cast<GeoDataDocument>(object)) {
                QFileInfo fileInfo(doc->fileName());
                if (fileInfo.fileName() == QLatin1StringView("ne_50m_admin_0_countries.pn2")) {
                    d->m_countryBoundaries = doc;
                    break;
                }
            }
        }
    }

    d->m_marbleWidget->setHighlightEnabled(true);

    if (d->m_countryBoundaries && d->m_countryNames) {
        d->m_countryNames->setVisible(false);
        d->m_marbleWidget->model()->treeModel()->updateFeature(d->m_countryNames);
        emit gameInitialized();
    }
}

void CountryByShape::postQuestion(QObject *gameObject)
{
    // Find a random placemark

    Q_ASSERT_X(d->m_countryNames, "CountryByShape::postQuestion", "CountryByShapePrivate::m_countryNames is NULL");

    QList<GeoDataPlacemark *> countryPlacemarks = d->m_countryNames->placemarkList();

    uint randomSeed = uint(QTime::currentTime().msec());
    QRandomGenerator::global()->seed(randomSeed);

    bool found = false;
    GeoDataPlacemark *placemark = nullptr;
    GeoDataPoint *point = nullptr;
    GeoDataCoordinates coord;
    GeoDataLatLonAltBox box;
    QVariantList answerOptions;
    while (!found) {
        int randomIndex = QRandomGenerator::global()->generate() % countryPlacemarks.size();
        placemark = countryPlacemarks[randomIndex];
        point = dynamic_cast<GeoDataPoint *>(placemark->geometry());
        coord = point->coordinates();

        if (point) {
            /**
             * Find the country geometry and fetch corresponding
             * GeoDataLatLonAltBox to zoom in to that country so that
             * it fills the viewport.
             */

            Q_ASSERT_X(d->m_countryBoundaries, "CountryByShape::postQuestion", "CountryByShapePrivate::m_countryBoundaries is NULL");

            QList<GeoDataFeature *>::Iterator i = d->m_countryBoundaries->begin();
            QList<GeoDataFeature *>::Iterator const end = d->m_countryBoundaries->end();
            for (; i != end; ++i) {
                GeoDataPlacemark *country = static_cast<GeoDataPlacemark *>(*i);

                GeoDataPolygon *polygon = dynamic_cast<GeoDataPolygon *>(country->geometry());
                GeoDataLinearRing *linearring = dynamic_cast<GeoDataLinearRing *>(country->geometry());
                GeoDataMultiGeometry *multigeom = dynamic_cast<GeoDataMultiGeometry *>(country->geometry());

                if (polygon && polygon->contains(coord) && !d->m_continentsAndOceans.contains(country->name(), Qt::CaseSensitive)) {
                    box = polygon->latLonAltBox();
                    found = true;
                    break;
                }
                if (linearring && linearring->contains(coord) && !d->m_continentsAndOceans.contains(country->name(), Qt::CaseSensitive)) {
                    box = linearring->latLonAltBox();
                    found = true;
                    break;
                }
                if (multigeom) {
                    QList<GeoDataGeometry *>::Iterator iter = multigeom->begin();
                    QList<GeoDataGeometry *>::Iterator const end = multigeom->end();

                    for (; iter != end; ++iter) {
                        GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon *>(*iter);
                        if (poly && poly->contains(coord) && !d->m_continentsAndOceans.contains(country->name(), Qt::CaseSensitive)) {
                            box = poly->latLonAltBox();
                            found = true;
                            break;
                        }
                    }
                }
                if (found) {
                    break;
                }
            }
        }
    }
    d->m_marbleWidget->setHighlightEnabled(true);
    emit announceHighlight(coord.longitude(GeoDataCoordinates::Degree), coord.latitude(GeoDataCoordinates::Degree), GeoDataCoordinates::Degree);

    /**
     * Now disable the highlight feature so that
     * the user click doesn't disturb the highlight
     * we did to ask question.
     */
    d->m_marbleWidget->setHighlightEnabled(false);

    d->m_marbleWidget->centerOn(box, true);

    answerOptions << placemark->name() << countryPlacemarks[QRandomGenerator::global()->generate() % countryPlacemarks.size()]->name()
                  << countryPlacemarks[QRandomGenerator::global()->generate() % countryPlacemarks.size()]->name()
                  << countryPlacemarks[QRandomGenerator::global()->generate() % countryPlacemarks.size()]->name();

    // Randomize options in list answerOptions
    for (int i = 0; i < answerOptions.size(); ++i) {
        QVariant option = answerOptions.takeAt(QRandomGenerator::global()->generate() % answerOptions.size());
        answerOptions.append(option);
    }

    if (gameObject) {
        QMetaObject::invokeMethod(gameObject, "countryByShapeQuestion", Q_ARG(QVariant, QVariant(answerOptions)), Q_ARG(QVariant, QVariant(placemark->name())));
    }
}

} // namespace Marble

#include "moc_CountryByShape.cpp"

// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

// Self
#include "CountryByFlag.h"

// Qt
#include <QImage>
#include <QList>
#include <QRandomGenerator>
#include <QString>
#include <QTime>
#include <QVariant>
#include <QVariantList>

// Marble
#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleDebug.h>
#include <marble/MarbleDirs.h>
#include <marble/MarbleModel.h>
#include <marble/MarblePlacemarkModel.h>
#include <marble/MarbleWidget.h>

#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>

namespace Marble
{
class CountryByFlagPrivate
{
public:
    CountryByFlagPrivate(MarbleWidget *marbleWidget)
        : m_marbleWidget(marbleWidget)
        , m_parent(nullptr)
        , m_countryNames(nullptr)
    {
        m_continentsAndOceans << QStringLiteral("Asia") << QStringLiteral("Africa") << QStringLiteral("North America") << QStringLiteral("South America")
                              << QStringLiteral("Antarctica") << QStringLiteral("Europe") << QStringLiteral("Australia") << QStringLiteral("Arctic Ocean")
                              << QStringLiteral("Indian Ocean") << QStringLiteral("North Atlantic Ocean") << QStringLiteral("North Pacific Ocean")
                              << QStringLiteral("South Pacific Ocean") << QStringLiteral("South Atlantic Ocean") << QStringLiteral("Southern Ocean");
    }

    MarbleWidget *m_marbleWidget;
    CountryByFlag *m_parent;

    /**
     * Document to store point placemarks which
     * have country names ( from file "boundaryplacemarks.cache" )
     */
    GeoDataDocument *m_countryNames;

    /*
     * When I select a random placemark form boundaryplacemarks.cache
     * it may represent a continent. Since there is no flag
     * for a continent, we will not use this placemark to post question.
     * This list will help checking whether the placemark chosen to
     * post question is a continent/ocean .
     */
    QStringList m_continentsAndOceans;
};

CountryByFlag::CountryByFlag(MarbleWidget *marbleWidget)
    : QObject()
    , d(new CountryByFlagPrivate(marbleWidget))
{
    d->m_parent = this;
}

CountryByFlag::~CountryByFlag()
{
    delete d->m_countryNames;
    delete d;
}

void CountryByFlag::initiateGame()
{
    /**
     * First remove the GeoDataDocument, which displays
     * country names, from map.
     */

    if (!d->m_countryNames) {
        const GeoDataTreeModel *const treeModel = d->m_marbleWidget->model()->treeModel();
        for (int i = 0; i < treeModel->rowCount(); ++i) {
            QVariant const data = treeModel->data(treeModel->index(i, 0), MarblePlacemarkModel::ObjectPointerRole);
            GeoDataObject *object = qvariant_cast<GeoDataObject *>(data);
            Q_ASSERT_X(object, "CountryByFlag::initiateGame", "failed to get valid data from treeModel for GeoDataObject");
            if (auto doc = geodata_cast<GeoDataDocument>(object)) {
                QFileInfo fileInfo(doc->fileName());
                if (fileInfo.fileName() == QLatin1StringView("boundaryplacemarks.cache")) {
                    d->m_countryNames = doc;
                    break;
                }
            }
        }
    }

    if (d->m_countryNames) {
        d->m_countryNames->setVisible(false);
        d->m_marbleWidget->model()->treeModel()->updateFeature(d->m_countryNames);
        d->m_marbleWidget->centerOn(23.0, 42.0);
        d->m_marbleWidget->setDistance(7500);
        d->m_marbleWidget->setHighlightEnabled(false);
        emit gameInitialized();
    }
}

void CountryByFlag::postQuestion(QObject *gameObject)
{
    /**
     * Find a random placemark
     */
    Q_ASSERT_X(d->m_countryNames, "CountryByFlag::postQuestion", "CountryByFlagPrivate::m_countryNames is NULL");
    QList<GeoDataPlacemark *> countryPlacemarks = d->m_countryNames->placemarkList();

    uint randomSeed = uint(QTime::currentTime().msec());
    QRandomGenerator::global()->seed(randomSeed);

    bool found = false;
    GeoDataPlacemark *placemark = nullptr;
    QVariantList answerOptions;
    QString flagPath;

    while (!found) {
        int randomIndex = QRandomGenerator::global()->generate() % countryPlacemarks.size();
        placemark = countryPlacemarks[randomIndex];

        if (!d->m_continentsAndOceans.contains(placemark->name(), Qt::CaseSensitive)) {
            const QString countryCode = placemark->countryCode().toLower();
            flagPath = MarbleDirs::path(QLatin1StringView("flags/flag_") + countryCode + QLatin1StringView(".svg"));
            QImage flag = QFile::exists(flagPath) ? QImage(flagPath) : QImage();
            if (!flag.isNull()) {
                flagPath = QLatin1StringView("../../../data/flags/flag_") + countryCode + QLatin1StringView(".svg");
                found = true;
            }
        }
    }

    answerOptions << placemark->name() << countryPlacemarks[QRandomGenerator::global()->generate() % countryPlacemarks.size()]->name()
                  << countryPlacemarks[QRandomGenerator::global()->generate() % countryPlacemarks.size()]->name()
                  << countryPlacemarks[QRandomGenerator::global()->generate() % countryPlacemarks.size()]->name();

    // Randomize the options in the list answerOptions
    for (int i = 0; i < answerOptions.size(); ++i) {
        QVariant option = answerOptions.takeAt(QRandomGenerator::global()->generate() % answerOptions.size());
        answerOptions.append(option);
    }
    if (gameObject) {
        QMetaObject::invokeMethod(gameObject,
                                  "countryByFlagQuestion",
                                  Q_ARG(QVariant, QVariant(answerOptions)),
                                  Q_ARG(QVariant, QVariant(flagPath)),
                                  Q_ARG(QVariant, QVariant(placemark->name())));
    }
}

} // namespace Marble

#include "moc_CountryByFlag.cpp"

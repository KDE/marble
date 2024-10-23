// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#include "FileLoader.h"

#include <QBuffer>
#include <QDataStream>
#include <QFile>

#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataFolder.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataLineStyle.h"
#include "GeoDataParser.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataPolygon.h"
#include "GeoDataRelation.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTour.h"
#include "GeoDataTrack.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"

namespace Marble
{

class FileLoaderPrivate
{
public:
    FileLoaderPrivate(FileLoader *parent,
                      const PluginManager *pluginManager,
                      bool recenter,
                      const QString &file,
                      const QString &property,
                      const GeoDataStyle::Ptr &style,
                      DocumentRole role,
                      int renderOrder)
        : q(parent)
        , m_runner(pluginManager)
        , m_filepath(file)
        , m_property(property)
        , m_style(style)
        , m_styleMap(new GeoDataStyleMap)
        , m_document(nullptr)
        , m_renderOrder(renderOrder)
        , m_documentRole(role)
        , m_recenter(recenter)
    {
        if (m_style) {
            m_styleMap->setId(QStringLiteral("default-map"));
            m_styleMap->insert(QStringLiteral("normal"), QLatin1Char('#') + m_style->id());
        }
    }

    FileLoaderPrivate(FileLoader *parent, const PluginManager *pluginManager, const QString &contents, const QString &file, DocumentRole role)
        : q(parent)
        , m_runner(pluginManager)
        , m_filepath(file)
        , m_contents(contents)
        , m_styleMap(nullptr)
        , m_document(nullptr)
        , m_documentRole(role)
        , m_recenter(false)
    {
    }

    ~FileLoaderPrivate()
    {
        delete m_styleMap;
    }

    void createFilterProperties(GeoDataContainer *container);
    static int cityPopIdx(qint64 population);
    static int spacePopIdx(qint64 population);
    static int areaPopIdx(qreal area);

    void documentParsed(GeoDataDocument *doc, const QString &error);

    FileLoader *q;
    ParsingRunnerManager m_runner;
    QString m_filepath;
    QString m_contents;
    QString m_property;
    GeoDataStyle::Ptr m_style;
    GeoDataStyleMap *m_styleMap;
    GeoDataDocument *m_document;
    QString m_error;
    int m_renderOrder;
    DocumentRole m_documentRole;
    bool m_recenter;
};

FileLoader::FileLoader(QObject *parent,
                       const PluginManager *pluginManager,
                       bool recenter,
                       const QString &file,
                       const QString &property,
                       const GeoDataStyle::Ptr &style,
                       DocumentRole role,
                       int renderOrder)
    : QThread(parent)
    , d(new FileLoaderPrivate(this, pluginManager, recenter, file, property, style, role, renderOrder))
{
}

FileLoader::FileLoader(QObject *parent, const PluginManager *pluginManager, const QString &contents, const QString &file, DocumentRole role)
    : QThread(parent)
    , d(new FileLoaderPrivate(this, pluginManager, contents, file, role))
{
}

FileLoader::~FileLoader()
{
    delete d;
}

QString FileLoader::path() const
{
    return d->m_filepath;
}

GeoDataDocument *FileLoader::document()
{
    return d->m_document;
}

QString FileLoader::error() const
{
    return d->m_error;
}

void FileLoader::run()
{
    if (d->m_contents.isEmpty()) {
        QString defaultSourceName;

        mDebug() << "starting parser for" << d->m_filepath;

        QFileInfo fileinfo(d->m_filepath);
        QString path = fileinfo.path();
        if (path == QLatin1StringView("."))
            path.clear();
        QString name = fileinfo.completeBaseName();
        QString suffix = fileinfo.suffix();

        // determine source, cache names
        if (fileinfo.isAbsolute()) {
            // We got an _absolute_ path now: e.g. "/patrick.kml"
            defaultSourceName = path + QLatin1Char('/') + name + QLatin1Char('.') + suffix;
        } else if (d->m_filepath.contains(QLatin1Char('/'))) {
            // _relative_ path: "maps/mars/viking/patrick.kml"
            defaultSourceName = MarbleDirs::path(path + QLatin1Char('/') + name + QLatin1Char('.') + suffix);
            if (!QFile::exists(defaultSourceName)) {
                defaultSourceName = MarbleDirs::path(path + QLatin1Char('/') + name + QLatin1StringView(".cache"));
            }
        } else {
            // _standard_ shared placemarks: "placemarks/patrick.kml"
            defaultSourceName = MarbleDirs::path(QLatin1StringView("placemarks/") + path + name + QLatin1Char('.') + suffix);
            if (!QFile::exists(defaultSourceName)) {
                defaultSourceName = MarbleDirs::path(QLatin1StringView("placemarks/") + path + name + QLatin1StringView(".cache"));
            }
        }

        if (QFile::exists(defaultSourceName)) {
            mDebug() << "No recent Default Placemark Cache File available!";

            // use runners: pnt, gpx, osm
            connect(&d->m_runner, SIGNAL(parsingFinished(GeoDataDocument *, QString)), this, SLOT(documentParsed(GeoDataDocument *, QString)));
            d->m_runner.parseFile(defaultSourceName, d->m_documentRole);
        } else {
            mDebug() << "No Default Placemark Source File for " << name;
        }
        // content is not empty, we load from data
    } else {
        // Read the KML Data
        GeoDataParser parser(GeoData_KML);

        QByteArray ba(d->m_contents.toUtf8());
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadOnly);

        if (!parser.read(&buffer)) {
            qWarning("Could not import kml buffer!");
            Q_EMIT loaderFinished(this);
            return;
        }

        GeoDocument *document = parser.releaseDocument();
        Q_ASSERT(document);

        d->m_document = static_cast<GeoDataDocument *>(document);
        d->m_document->setProperty(d->m_property);
        d->m_document->setDocumentRole(d->m_documentRole);
        d->createFilterProperties(d->m_document);
        buffer.close();

        mDebug() << "newGeoDataDocumentAdded" << d->m_filepath;

        Q_EMIT newGeoDataDocumentAdded(d->m_document);
        Q_EMIT loaderFinished(this);
    }
}

bool FileLoader::recenter() const
{
    return d->m_recenter;
}

void FileLoaderPrivate::documentParsed(GeoDataDocument *doc, const QString &error)
{
    m_error = error;
    if (doc) {
        m_document = doc;
        doc->setProperty(m_property);
        if (m_style) {
            doc->addStyleMap(*m_styleMap);
            doc->addStyle(m_style);
        }

        if (m_renderOrder != 0) {
            for (GeoDataPlacemark *placemark : doc->placemarkList()) {
                if (auto polygon = geodata_cast<GeoDataPolygon>(placemark->geometry())) {
                    polygon->setRenderOrder(m_renderOrder);
                }
            }
        }

        createFilterProperties(doc);
        Q_EMIT q->newGeoDataDocumentAdded(m_document);
    }
    Q_EMIT q->loaderFinished(q);
}

void FileLoaderPrivate::createFilterProperties(GeoDataContainer *container)
{
    const QString styleUrl = QLatin1Char('#') + m_styleMap->id();

    QList<GeoDataFeature *>::Iterator i = container->begin();
    QList<GeoDataFeature *>::Iterator const end = container->end();
    for (; i != end; ++i) {
        if (auto child = dynamic_cast<GeoDataContainer *>(*i)) {
            createFilterProperties(child);
        } else if (geodata_cast<GeoDataTour>(*i) || geodata_cast<GeoDataRelation>(*i) || geodata_cast<GeoDataGroundOverlay>(*i)
                   || geodata_cast<GeoDataPhotoOverlay>(*i) || geodata_cast<GeoDataScreenOverlay>(*i)) {
            /** @todo: How to handle this ? */
        } else if (auto placemark = geodata_cast<GeoDataPlacemark>(*i)) {
            const QString placemarkRole = placemark->role();
            Q_ASSERT(placemark->geometry());

            bool hasPopularity = false;

            if (!geodata_cast<GeoDataTrack>(placemark->geometry()) && !geodata_cast<GeoDataPoint>(placemark->geometry()) && m_documentRole == MapDocument
                && m_style) {
                placemark->setStyleUrl(styleUrl);
            }

            // Mountain (H), Volcano (V), Shipwreck (W)
            if (placemarkRole == QLatin1StringView("H") || placemarkRole == QLatin1StringView("V") || placemarkRole == QLatin1StringView("W")) {
                qreal altitude = placemark->coordinate().altitude();
                if (altitude != 0.0) {
                    hasPopularity = true;
                    placemark->setPopularity((qint64)(altitude * 1000.0));
                    placemark->setZoomLevel(cityPopIdx(qAbs((qint64)(altitude * 1000.0))));
                }
            }
            // Continent (K), Ocean (O), Nation (S)
            else if (placemarkRole == QLatin1StringView("K") || placemarkRole == QLatin1StringView("O") || placemarkRole == QLatin1StringView("S")) {
                qreal area = placemark->area();
                if (area >= 0.0) {
                    hasPopularity = true;
                    //                mDebug() << placemark->name() << " " << (qint64)(area);
                    placemark->setPopularity((qint64)(area * 100));
                    placemark->setZoomLevel(areaPopIdx(area));
                }
            }
            // Pole (P)
            else if (placemarkRole == QLatin1StringView("P")) {
                placemark->setPopularity(1000000000);
                placemark->setZoomLevel(1);
            }
            // Magnetic Pole (M)
            else if (placemarkRole == QLatin1StringView("M")) {
                placemark->setPopularity(10000000);
                placemark->setZoomLevel(3);
            }
            // MannedLandingSite (h)
            else if (placemarkRole == QLatin1StringView("h")) {
                placemark->setPopularity(1000000000);
                placemark->setZoomLevel(1);
            }
            // RoboticRover (r)
            else if (placemarkRole == QLatin1StringView("r")) {
                placemark->setPopularity(10000000);
                placemark->setZoomLevel(2);
            }
            // UnmannedSoftLandingSite (u)
            else if (placemarkRole == QLatin1StringView("u")) {
                placemark->setPopularity(1000000);
                placemark->setZoomLevel(3);
            }
            // UnmannedSoftLandingSite (i)
            else if (placemarkRole == QLatin1StringView("i")) {
                placemark->setPopularity(1000000);
                placemark->setZoomLevel(3);
            }
            // Space Terrain: Craters, Maria, Montes, Valleys, etc.
            else if (placemarkRole == QLatin1StringView("m") || placemarkRole == QLatin1StringView("v") || placemarkRole == QLatin1StringView("o")
                     || placemarkRole == QLatin1StringView("c") || placemarkRole == QLatin1StringView("a")) {
                qint64 diameter = placemark->population();
                if (diameter >= 0) {
                    hasPopularity = true;
                    placemark->setPopularity(diameter);
                    if (placemarkRole == QLatin1StringView("c")) {
                        placemark->setZoomLevel(spacePopIdx(diameter));
                        if (placemark->name() == QLatin1StringView("Tycho") || placemark->name() == QLatin1StringView("Copernicus")) {
                            placemark->setZoomLevel(1);
                        }
                    } else {
                        placemark->setZoomLevel(spacePopIdx(diameter));
                    }

                    if (placemarkRole == QLatin1StringView("a") && diameter == 0) {
                        placemark->setPopularity(1000000000);
                        placemark->setZoomLevel(1);
                    }
                }
            } else {
                qint64 population = placemark->population();
                if (population >= 0) {
                    hasPopularity = true;
                    placemark->setPopularity(population);
                    placemark->setZoomLevel(cityPopIdx(population));
                }
            }

            //  Then we set the visual category:

            if (placemarkRole == QLatin1StringView("H"))
                placemark->setVisualCategory(GeoDataPlacemark::Mountain);
            else if (placemarkRole == QLatin1StringView("V"))
                placemark->setVisualCategory(GeoDataPlacemark::Volcano);

            else if (placemarkRole == QLatin1StringView("m"))
                placemark->setVisualCategory(GeoDataPlacemark::Mons);
            else if (placemarkRole == QLatin1StringView("v"))
                placemark->setVisualCategory(GeoDataPlacemark::Valley);
            else if (placemarkRole == QLatin1StringView("o"))
                placemark->setVisualCategory(GeoDataPlacemark::OtherTerrain);
            else if (placemarkRole == QLatin1StringView("c"))
                placemark->setVisualCategory(GeoDataPlacemark::Crater);
            else if (placemarkRole == QLatin1StringView("a"))
                placemark->setVisualCategory(GeoDataPlacemark::Mare);

            else if (placemarkRole == QLatin1StringView("P"))
                placemark->setVisualCategory(GeoDataPlacemark::GeographicPole);
            else if (placemarkRole == QLatin1StringView("M"))
                placemark->setVisualCategory(GeoDataPlacemark::MagneticPole);
            else if (placemarkRole == QLatin1StringView("W"))
                placemark->setVisualCategory(GeoDataPlacemark::ShipWreck);
            else if (placemarkRole == QLatin1StringView("F"))
                placemark->setVisualCategory(GeoDataPlacemark::AirPort);
            else if (placemarkRole == QLatin1StringView("A"))
                placemark->setVisualCategory(GeoDataPlacemark::Observatory);
            else if (placemarkRole == QLatin1StringView("K"))
                placemark->setVisualCategory(GeoDataPlacemark::Continent);
            else if (placemarkRole == QLatin1StringView("O"))
                placemark->setVisualCategory(GeoDataPlacemark::Ocean);
            else if (placemarkRole == QLatin1StringView("S"))
                placemark->setVisualCategory(GeoDataPlacemark::Nation);
            else if (placemarkRole == QLatin1StringView("PPL") || placemarkRole == QLatin1StringView("PPLF") || placemarkRole == QLatin1StringView("PPLG")
                     || placemarkRole == QLatin1StringView("PPLL") || placemarkRole == QLatin1StringView("PPLQ") || placemarkRole == QLatin1StringView("PPLR")
                     || placemarkRole == QLatin1StringView("PPLS") || placemarkRole == QLatin1StringView("PPLW")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeCity);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigCity);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumCity);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallCity);
                    break;
                }
            } else if (placemarkRole == QLatin1StringView("PPLA")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeStateCapital);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigStateCapital);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumStateCapital);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallStateCapital);
                    break;
                }
            } else if (placemarkRole == QLatin1StringView("PPLC")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeNationCapital);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigNationCapital);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumNationCapital);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallNationCapital);
                    break;
                }
            } else if (placemarkRole == QLatin1StringView("PPLA2") || placemarkRole == QLatin1StringView("PPLA3")
                       || placemarkRole == QLatin1StringView("PPLA4")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeCountyCapital);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigCountyCapital);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumCountyCapital);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallCountyCapital);
                    break;
                }
            } else if (placemarkRole == QLatin1StringView(" ") && !hasPopularity && placemark->visualCategory() == GeoDataPlacemark::Unknown) {
                placemark->setVisualCategory(GeoDataPlacemark::Unknown); // default location
                placemark->setZoomLevel(0);
            } else if (placemarkRole == QLatin1StringView("h")) {
                placemark->setVisualCategory(GeoDataPlacemark::MannedLandingSite);
            } else if (placemarkRole == QLatin1StringView("r")) {
                placemark->setVisualCategory(GeoDataPlacemark::RoboticRover);
            } else if (placemarkRole == QLatin1StringView("u")) {
                placemark->setVisualCategory(GeoDataPlacemark::UnmannedSoftLandingSite);
            } else if (placemarkRole == QLatin1StringView("i")) {
                placemark->setVisualCategory(GeoDataPlacemark::UnmannedHardLandingSite);
            }

            // At last fine-tune zoomlevel:
            if (!placemark->isVisible()) {
                placemark->setZoomLevel(18);
            }
            // Workaround: Emulate missing "setVisible" serialization by allowing for population
            // values smaller than -1 which are considered invisible.
            else if (placemark->population() < -1) {
                placemark->setZoomLevel(18);
            } else if (placemarkRole == QLatin1StringView("W")) {
                if (placemark->zoomLevel() < 4) {
                    placemark->setZoomLevel(4);
                }
            } else if (placemarkRole == QLatin1StringView("O")) {
                placemark->setZoomLevel(2);
            } else if (placemarkRole == QLatin1StringView("K")) {
                placemark->setZoomLevel(0);
            }
        } else {
            qWarning() << "Unknown feature" << (*i)->nodeType() << ". Skipping.";
        }
    }
}

int FileLoaderPrivate::cityPopIdx(qint64 population)
{
    int popidx = 3;

    if (population < 2500)
        popidx = 10;
    else if (population < 5000)
        popidx = 9;
    else if (population < 25000)
        popidx = 8;
    else if (population < 75000)
        popidx = 7;
    else if (population < 250000)
        popidx = 6;
    else if (population < 750000)
        popidx = 5;
    else if (population < 2500000)
        popidx = 4;

    return popidx;
}

int FileLoaderPrivate::spacePopIdx(qint64 population)
{
    int popidx = 1;

    if (population < 1000)
        popidx = 10;
    else if (population < 2000)
        popidx = 9;
    else if (population < 8000)
        popidx = 8;
    else if (population < 20000)
        popidx = 7;
    else if (population < 60000)
        popidx = 6;
    else if (population < 100000)
        popidx = 5;
    else if (population < 200000)
        popidx = 4;
    else if (population < 400000)
        popidx = 2;
    else if (population < 600000)
        popidx = 1;

    return popidx;
}

int FileLoaderPrivate::areaPopIdx(qreal area)
{
    int popidx = 1;
    if (area < 200000)
        popidx = 5;
    else if (area < 1000000)
        popidx = 4;
    else if (area < 2500000)
        popidx = 3;
    else if (area < 5000000)
        popidx = 2;

    return popidx;
}

} // namespace Marble

#include "moc_FileLoader.cpp"

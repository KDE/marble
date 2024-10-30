// SPDX-FileCopyrightText: 2014 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "thumbnailer.h"

// Marble
#include <FileManager.h>
#include <GeoDataDocument.h>
#include <GeoDataLatLonAltBox.h>
#include <GeoDataTreeModel.h>
#include <GeoPainter.h>
#include <MarbleModel.h>
#include <RenderPlugin.h>
#include <ViewportParams.h>

static const int timeoutTime = 5000; // in msec

namespace Marble
{

GeoDataThumbnailer::GeoDataThumbnailer()
    : ThumbnailCreator(nullptr, QVariantList())
{
    m_marbleMap.setMapThemeId(QStringLiteral("earth/openstreetmap/openstreetmap.dgml"));
    m_marbleMap.setProjection(Equirectangular);
    m_marbleMap.setMapQualityForViewContext(PrintQuality, Still);
    m_marbleMap.setViewContext(Still);
    for (RenderPlugin *plugin : m_marbleMap.renderPlugins()) {
        plugin->setEnabled(false);
    }

    m_outtimer.setInterval(timeoutTime);
    m_outtimer.setSingleShot(true);
    connect(&m_outtimer, &QTimer::timeout, &m_eventLoop, &QEventLoop::quit);

    MarbleModel *const model = m_marbleMap.model();
    connect(model->treeModel(), &GeoDataTreeModel::added, this, &GeoDataThumbnailer::onGeoDataObjectAdded);
    connect(model->fileManager(), &FileManager::fileError, this, [this](const QString & /*path*/, const QString & /*error*/) {
        m_hadErrors = true;
        m_outtimer.stop();
        m_eventLoop.quit();
    });
}

GeoDataThumbnailer::~GeoDataThumbnailer() = default;

ThumbnailResult GeoDataThumbnailer::create(const ThumbnailRequest &request)
{
    m_marbleMap.setSize(request.targetSize());

    MarbleModel *const model = m_marbleMap.model();

    // load the document content
    m_loadingCompleted = false;
    m_hadErrors = false;

    QString path = request.url().toLocalFile();

    m_currentFilename = path;
    model->addGeoDataFile(path);

    if ((!m_loadingCompleted) && (!m_hadErrors)) {
        // loading is done async, so wait here for a while
        // Using a QEventLoop here seems fine, thumbnailers are only used inside the
        // thumbnail protocol slave, it seems
        m_outtimer.start();
        m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

    QImage image;

    if (m_loadingCompleted) {
        // TODO: limit to shown map, if full earth is used
        image = QImage(request.targetSize(), QImage::Format_ARGB32);
        image.fill(qRgba(0, 0, 0, 0));

        // Create a painter that will do the painting.
        GeoPainter geoPainter(&image, m_marbleMap.viewport(), m_marbleMap.mapQuality());

        m_marbleMap.paint(geoPainter, QRect()); // TODO: dirtyRect seems currently unused, make sure it is
    }

    ThumbnailResult result = ThumbnailResult::pass(image);

    model->removeGeoData(path);
    m_currentFilename.clear();

    return result;
}

static qreal radius(qreal zoom)
{
    return pow(M_E, (zoom / 200.0));
}

void GeoDataThumbnailer::onGeoDataObjectAdded(GeoDataObject *object)
{
    const auto document = geodata_cast<GeoDataDocument>(object);

    if (!document) {
        return;
    }

    if (document->fileName() != m_currentFilename) {
        return;
    }

    const GeoDataLatLonAltBox latLonAltBox = document->latLonAltBox();
    const GeoDataCoordinates center = latLonAltBox.center();

    int newRadius = m_marbleMap.radius();
    // prevent divide by zero
    if (latLonAltBox.height() && latLonAltBox.width()) {
        const ViewportParams *viewparams = m_marbleMap.viewport();
        // work out the needed zoom level
        const int horizontalRadius = (0.25 * M_PI) * (viewparams->height() / latLonAltBox.height());
        const int verticalRadius = (0.25 * M_PI) * (viewparams->width() / latLonAltBox.width());
        newRadius = qMin<int>(horizontalRadius, verticalRadius);
        newRadius = qMax<int>(radius(m_marbleMap.minimumZoom()), qMin<int>(newRadius, radius(m_marbleMap.maximumZoom())));
    }

    m_marbleMap.centerOn(center.longitude(GeoDataCoordinates::Degree), center.latitude(GeoDataCoordinates::Degree));

    m_marbleMap.setRadius(newRadius);

    m_loadingCompleted = true;
    m_outtimer.stop();
    m_eventLoop.quit();
}

}

#include "moc_thumbnailer.cpp"

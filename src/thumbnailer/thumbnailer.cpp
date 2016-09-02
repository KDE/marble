// Copyright 2014 Friedrich W. H. Kossebau <kossebau@kde.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "thumbnailer.h"

// Marble
#include <MarbleModel.h>
#include <GeoDataDocument.h>
#include <GeoPainter.h>
#include <GeoDataLatLonAltBox.h>
#include <ViewportParams.h>
#include <RenderPlugin.h>
#include <GeoDataTreeModel.h>
#include <GeoDataTypes.h>
// Qt
#include <QPainter>

static const int timeoutTime = 5000; // in msec

namespace Marble {

GeoDataThumbnailer::GeoDataThumbnailer()
  : ThumbCreator()
  , m_marbleMap()
{
    m_marbleMap.setMapThemeId(QStringLiteral("earth/openstreetmap/openstreetmap.dgml"));
    m_marbleMap.setProjection(Equirectangular);
    m_marbleMap.setMapQualityForViewContext( PrintQuality, Still );
    m_marbleMap.setViewContext( Still );
    foreach( RenderPlugin* plugin, m_marbleMap.renderPlugins() ) {
        plugin->setEnabled( false );
    }

    m_outtimer.setInterval(timeoutTime);
    m_outtimer.setSingleShot(true);
    connect(&m_outtimer, SIGNAL(timeout()), &m_eventLoop, SLOT(quit()));
}


GeoDataThumbnailer::~GeoDataThumbnailer()
{
}

bool GeoDataThumbnailer::create(const QString &path, int width, int height, QImage &image)
{
    m_marbleMap.setSize(width, height);

    MarbleModel *const model = m_marbleMap.model();

    // load the document content
    m_loadingCompleted = false;

    m_currentFilename = path;
    connect(model->treeModel(), SIGNAL(added(GeoDataObject*)), this, SLOT(onGeoDataObjectAdded(GeoDataObject*)));
    model->addGeoDataFile(path);

    if (! m_loadingCompleted) {
        // loading is done async, so wait here for a while
        // Using a QEventLoop here seems fine, thumbnailers are only used inside the
        // thumbnail protocol slave, it seems
        m_outtimer.start();
        m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

    if (m_loadingCompleted) {
        // TODO: limit to shown map, if full earth is used
        image = QImage(width, height, QImage::Format_ARGB32);
        image.fill(qRgba(0, 0, 0, 0));

        // Create a painter that will do the painting.
        GeoPainter geoPainter( &image, m_marbleMap.viewport(),
                               m_marbleMap.mapQuality() );

        m_marbleMap.paint( geoPainter, QRect() ); // TODO: dirtyRect seems currently unused, make sure it is
    }

    disconnect(model->treeModel(), SIGNAL(added(GeoDataObject*)), this, SLOT(onGeoDataObjectAdded(GeoDataObject*)));
    model->removeGeoData(path);
    m_currentFilename.clear();

    return m_loadingCompleted;
}

static qreal radius(qreal zoom)
{
    return pow(M_E, (zoom / 200.0));
}

void GeoDataThumbnailer::onGeoDataObjectAdded( GeoDataObject* object )
{
    if ( object->nodeType() != GeoDataTypes::GeoDataDocumentType ) {
        return;
    }

    const GeoDataDocument *document = static_cast<GeoDataDocument*>(object);
    if (document->fileName() != m_currentFilename) {
        return;
    }

    const GeoDataLatLonAltBox latLonAltBox = document->latLonAltBox();
    const GeoDataCoordinates center = latLonAltBox.center();

    int newRadius = m_marbleMap.radius();
    //prevent divide by zero
    if( latLonAltBox.height() && latLonAltBox.width() ) {
        const ViewportParams* viewparams = m_marbleMap.viewport();
        //work out the needed zoom level
        const int horizontalRadius = ( 0.25 * M_PI ) * ( viewparams->height() / latLonAltBox.height() );
        const int verticalRadius = ( 0.25 * M_PI ) * ( viewparams->width() / latLonAltBox.width() );
        newRadius = qMin<int>( horizontalRadius, verticalRadius );
        newRadius = qMax<int>(radius(m_marbleMap.minimumZoom()), qMin<int>(newRadius, radius(m_marbleMap.maximumZoom())));
    }

    m_marbleMap.centerOn( center.longitude(GeoDataCoordinates::Degree), center.latitude(GeoDataCoordinates::Degree) );

    m_marbleMap.setRadius( newRadius );

    m_loadingCompleted = true;
    m_outtimer.stop();
    m_eventLoop.quit();
}

ThumbCreator::Flags GeoDataThumbnailer::flags() const
{
    return DrawFrame;
}

}

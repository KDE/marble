//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konrad Enzensberger <e.konrad@mpegcode.com>
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "FileReaderPositionProviderPlugin.h"

#include <QtCore/QTimer>
#include <QtCore/QFile>

#include "GeoDataCoordinates.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataTypes.h"
#include "GeoDataData.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataIconStyle.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

namespace Marble
{

class FileReaderPositionProviderPluginPrivate
{
public:
    FileReaderPositionProviderPluginPrivate();
    ~FileReaderPositionProviderPluginPrivate();

    void importKmlFromData();
    void createSimulationPlacemarks( GeoDataContainer *container );

    int   m_currentIndex;
    PositionProviderStatus m_status;
    GeoDataLineString m_lineString;
};

FileReaderPositionProviderPluginPrivate::FileReaderPositionProviderPluginPrivate() :
    m_currentIndex( 0 ), m_status( PositionProviderStatusAcquiring )
{
    // nothing to do
}

FileReaderPositionProviderPluginPrivate::~FileReaderPositionProviderPluginPrivate()
{
    m_lineString.clear();
}

void FileReaderPositionProviderPluginPrivate::importKmlFromData()
{
    GeoDataParser parser( GeoData_KML );

    QString filename = MarbleDirs::path( "routing" );
    filename += "/route.kml";
    QFile file( filename );
    if ( !file.exists() ) {
        mDebug() << "The GPS file reader plugin expects routing/route.kml to exist in the local marble dir.";
        mDebug() << "Create a route and exit Marble to have it created automatically.";
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    if ( !parser.read( &file ) ) {
        mDebug() << "Could not import kml file. No position updates will happen.";
        return;
    }

    GeoDocument* doc = parser.releaseDocument();
    if ( doc ) {
        GeoDataDocument* document = dynamic_cast<GeoDataDocument*>( doc );
        if ( document && document->size() > 0 ) {
            document->setDocumentRole( UserDocument );
            document->setFileName( filename );
            createSimulationPlacemarks( dynamic_cast<GeoDataDocument*>( &document->last() ) );
        }
    }

    file.close();
}

void FileReaderPositionProviderPluginPrivate::createSimulationPlacemarks( GeoDataContainer *container )
{
    if ( !container ) {
        return;
    }

    m_lineString.clear();

    QVector<GeoDataFolder*> folders = container->folderList();
    foreach( const GeoDataFolder *folder, folders ) {
        foreach( const GeoDataPlacemark *placemark, folder->placemarkList() ) {
            GeoDataGeometry* geometry = placemark->geometry();
            GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
            if ( lineString ) {
                m_lineString = *lineString;
                return;
            }
        }
    }

    foreach( const GeoDataPlacemark *placemark, container->placemarkList() ) {
        GeoDataGeometry* geometry = placemark->geometry();
        GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
        if ( lineString ) {
            m_lineString = *lineString;
            return;
        }
    }
}

QString FileReaderPositionProviderPlugin::name() const
{
    return tr( "File Reader Position ProviderPlugin" );
}

QString FileReaderPositionProviderPlugin::nameId() const
{
    return "FileReaderPositionProviderPlugin";
}

QString FileReaderPositionProviderPlugin::guiString() const
{
    return tr( "GPS Position Simulation (File Reader)" );
}

QString FileReaderPositionProviderPlugin::version() const
{
    return "1.0";
}

QString FileReaderPositionProviderPlugin::description() const
{
    return tr( "Reports the GPS position from a previously calculated route." );
}

QString FileReaderPositionProviderPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> FileReaderPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Konrad Enzensberger", "e.konrad@mpegcode.com" )
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

QIcon FileReaderPositionProviderPlugin::icon() const
{
    return QIcon();
}

PositionProviderPlugin* FileReaderPositionProviderPlugin::newInstance() const
{
    return new FileReaderPositionProviderPlugin;
}

PositionProviderStatus FileReaderPositionProviderPlugin::status() const
{
    return d->m_status;
}

GeoDataCoordinates FileReaderPositionProviderPlugin::position() const
{
    return d->m_lineString.at( d->m_currentIndex );
}

GeoDataAccuracy FileReaderPositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;

    // faked values
    result.level = GeoDataAccuracy::Detailed;
    result.horizontal = 10.0;
    result.vertical = 10.0;

    return result;
}

FileReaderPositionProviderPlugin::FileReaderPositionProviderPlugin() :
    d( new FileReaderPositionProviderPluginPrivate )
{
    // nothing to do
}

FileReaderPositionProviderPlugin::~FileReaderPositionProviderPlugin()
{
    delete d;
}

void FileReaderPositionProviderPlugin::initialize()
{
    d->m_status = PositionProviderStatusAcquiring;

    d->m_currentIndex = 0;
    d->importKmlFromData();

    QTimer::singleShot( 1000, this, SLOT( update() ) );
}

bool FileReaderPositionProviderPlugin::isInitialized() const
{
    return ( d->m_lineString.size() > 0 );
}

qreal FileReaderPositionProviderPlugin::speed() const
{
    /** @todo: calculate speed */
    return 0.0;
}

qreal FileReaderPositionProviderPlugin::direction() const
{
    /** @todo: calculate direction */
    return 0.0;
}

QDateTime FileReaderPositionProviderPlugin::timestamp() const
{
    return QDateTime::currentDateTime();
}

void FileReaderPositionProviderPlugin::update()
{
    PositionProviderStatus newStatus = PositionProviderStatusAvailable;

    if ( d->m_currentIndex >= 0 && d->m_currentIndex < d->m_lineString.size() ) {
        if ( newStatus != d->m_status ) {
            d->m_status = newStatus;
            emit statusChanged( newStatus );
        }

        if ( newStatus == PositionProviderStatusAvailable ) {
            emit positionChanged( position(), accuracy() );
        }

        ++d->m_currentIndex;

        if( d->m_currentIndex < d->m_lineString.size() ) {
            QTimer::singleShot( 1000, this, SLOT( update() ) );
        }
    }

    if ( !d->m_lineString.isEmpty() && d->m_currentIndex >= d->m_lineString.size() ) {
        // Repeat from start
        d->m_currentIndex = 0;
        update();
    }
}

} // namespace Marble

Q_EXPORT_PLUGIN2( FileReaderPositionProviderPlugin, Marble::FileReaderPositionProviderPlugin )

#include "FileReaderPositionProviderPlugin.moc"

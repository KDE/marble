//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011        Konrad Enzensberger <e.konrad@mpegcode.com>
//

#include "FileReaderPositionProviderPlugin.h"

#include <QTimer>
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

namespace Marble {

class GpsSimulationPluginPrivate
{
public:
     explicit GpsSimulationPluginPrivate();
    ~GpsSimulationPluginPrivate();
   
    const PositionProviderStatus& status() const;
    void setStatus( const PositionProviderStatus& newStatus );
	void importKmlFromData();
	void createSimulationPlacemarks( GeoDataContainer *container );
	bool getPlacemarkCoord(int i,GeoDataCoordinates& );
	int  getPlacemarkCount();

private:
    PositionProviderStatus m_status;
	QString m_filepath;
    QString m_contents;
    DocumentRole m_documentRole;
    GeoDataDocument *m_document;

public:
	QMap<int,GeoDataCoordinates> m_placemarks;
	int	  m_iCount;
};

GpsSimulationPluginPrivate::GpsSimulationPluginPrivate() :
        m_status( PositionProviderStatusAcquiring )
{
	m_iCount=0;
}

GpsSimulationPluginPrivate::~GpsSimulationPluginPrivate()
{
	m_placemarks.clear();
}

const PositionProviderStatus& GpsSimulationPluginPrivate::status() const
{
    return m_status;
}

void GpsSimulationPluginPrivate::setStatus( const PositionProviderStatus& newStatus )
{
    m_status = newStatus;
}

void GpsSimulationPluginPrivate::importKmlFromData()
{
    GeoDataParser parser( GeoData_UNKNOWN );

	QString filename = MarbleDirs::path("routing");
	filename += "/route.kml";
    QFile file( filename );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    if ( !parser.read( &file ) ) {
        qWarning( "Could not import kml file!" );
        return;
    }

        m_documentRole=UserDocument;

    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );

	m_filepath =filename;

    m_document = static_cast<GeoDataDocument*>( document );
    m_document->setDocumentRole( m_documentRole );
    m_document->setFileName( m_filepath );

	GeoDataContainer *container =  m_document;
	const QVector<GeoDataPlacemark*> placemarks = container->placemarkList();

	m_placemarks.clear();
	createSimulationPlacemarks(m_document);
	m_iCount = m_placemarks.count();

	file.close();

//    mDebug() << "newGeoDataDocumentAdded" << m_filepath;

 //   emit q->newGeoDataDocumentAdded( m_document );
}

void GpsSimulationPluginPrivate::createSimulationPlacemarks( GeoDataContainer *container )
{
	qreal lon=0;
    qreal lat=0;
    qreal alt=0;

	GeoDataCoordinates pos;

	const char* chType=NULL;
	const char* chFolder = GeoDataTypes::GeoDataFolderType;
	const char* chDoc = GeoDataTypes::GeoDataDocumentType;

	QString qsFolder(chFolder);
	QString qsDoc(chDoc);

    QVector<GeoDataFeature*>::Iterator i = container->begin();
    QVector<GeoDataFeature*>::Iterator const end = container->end();
    for (; i != end; ++i ) 
	{
		chType = (*i)->nodeType();
		QString qsType(chType);

        if ( qsType == qsFolder
             || qsType == qsDoc ) 
		{
            GeoDataContainer *child = static_cast<GeoDataContainer*>( *i );
            createSimulationPlacemarks( child );
        } else {
            GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( *i );

			if( dynamic_cast<GeoDataLineString*>( placemark->geometry() ) )
			{
				if(placemark->name()=="Route")
				{
					GeoDataLineString* gdl = dynamic_cast<GeoDataLineString*>( placemark->geometry() );
					GeoDataLineString Gdl;
					Gdl << *gdl;
					int y=Gdl.size();
					for(int x=0;x<y;x++)
					{
						pos = Gdl.at(x);
					//	placemark->setCoordinate(pos.longitude(),pos.latitude());
					//	m_placemarks.append(placemark);
						m_placemarks.insert(x,pos);
					}
				}

			}
			else if( dynamic_cast<GeoDataPolygon*>( placemark->geometry() ) ) 
			{
				GeoDataPolygon* gdl = dynamic_cast<GeoDataPolygon*>( placemark->geometry() );
				GeoDataLinearRing glr = gdl->outerBoundary();
				//if(glr.size()<=0)
				//	glr = gdl->innerBoundary();

				for(int x=0;x<glr.size();x++)
				{
					pos = glr.at(x);
					placemark->setCoordinate(pos.longitude(),pos.latitude());
			//		m_placemarks.append(placemark);
				}

			}
			else if( dynamic_cast<GeoDataIconStyle*>( placemark->geometry() ) ) 
			{
				pos = placemark->coordinate();
			
			}
			else
			{
				placemark->coordinate( lon, lat, alt );
			//	m_placemarks.append(placemark);
			}

            bool hasPopularity = false;
		}
	}
}

bool GpsSimulationPluginPrivate::getPlacemarkCoord(int i,GeoDataCoordinates& pos)
{

    qreal lon=0;
    qreal lat=0;
    qreal alt=0;

	if(i>=m_placemarks.count())return false;

	pos = m_placemarks.value(i);

	return true;
}
int GpsSimulationPluginPrivate::getPlacemarkCount()
{
	return m_iCount;
}

QString GpsSimulationPlugin::name() const
{
    return "Gps Simulation Plugin"; /** @todo FIXME I18N ... */
}

QString GpsSimulationPlugin::nameId() const
{
    return "GpsSimulationPlugin";
}

QString GpsSimulationPlugin::guiString() const
{
    return "GPS Simulation Location"; /** @todo FIXME I18N ... */
}

QString GpsSimulationPlugin::description() const
{
    return "Reports the GPS position from route"; /** @todo FIXME I18N ... */
}

QIcon GpsSimulationPlugin::icon() const
{
    return QIcon();
}

PositionProviderPlugin* GpsSimulationPlugin::newInstance() const
{
    return new GpsSimulationPlugin;
}

PositionProviderStatus GpsSimulationPlugin::status() const
{
    return d->status();
}

GeoDataCoordinates GpsSimulationPlugin::position() const
{
	GeoDataCoordinates pos;
	pos.setLongitude(m_lon,GeoDataCoordinates::Degree);
	pos.setLatitude(m_lat,GeoDataCoordinates::Degree);
    return pos;
}

GeoDataAccuracy GpsSimulationPlugin::accuracy() const
{
    GeoDataAccuracy result;

	//faked values
	result.level = GeoDataAccuracy::Detailed;
    result.horizontal = 10.0;
    result.vertical = 10.0;

    return result;
}

GpsSimulationPlugin::GpsSimulationPlugin() :
        d( new GpsSimulationPluginPrivate )
{
	m_pUpdateTimer = NULL;
	m_pUpdateTimer = new QTimer();
	m_pUpdateTimer->setSingleShot(true);
	m_pUpdateTimer->setInterval(1000);
	connect( m_pUpdateTimer, SIGNAL(timeout()), SLOT(update()) );

	m_lon=0.0;
	m_lat=0.0;
}

GpsSimulationPlugin::~GpsSimulationPlugin()
{
    delete d;
}

void GpsSimulationPlugin::initialize()
{
 
	PositionProviderStatus newStatus = PositionProviderStatusAcquiring;
	d->setStatus( newStatus );

	m_iPos=0;
	d->importKmlFromData();

	m_lon=0.0;
	m_lat=0.0;

	m_pUpdateTimer->start();
}

bool GpsSimulationPlugin::isInitialized() const
{
	if(d->getPlacemarkCount()>0)
		return true;
	else return false;
}

qreal GpsSimulationPlugin::speed() const
{
	//ToDo: calculate speed
    return 0.0;
}

qreal GpsSimulationPlugin::direction() const
{
   //ToDo: calculate direction
   return 0.0;
}

void GpsSimulationPlugin::update()
{
	PositionProviderStatus newStatus = PositionProviderStatusAvailable;
 
	m_pUpdateTimer->stop();

	GeoDataCoordinates pos;
	d->getPlacemarkCoord(m_iPos++,pos);

	m_lon=pos.longitude(GeoDataCoordinates::Degree);
	m_lat=pos.latitude(GeoDataCoordinates::Degree);

	if ( newStatus != d->status() ) {
		d->setStatus( newStatus );
		emit statusChanged( newStatus );
	}

	if ( newStatus == PositionProviderStatusAvailable ) {
		emit positionChanged( position(), accuracy() );
	}

	if(d->getPlacemarkCount()>m_iPos)
		m_pUpdateTimer->start();
}

void GpsSimulationPlugin::positionProviderDisabled(bool set)
{
	if(!set)
	{
		d->m_placemarks.clear();
		d->m_iCount=0;
		m_iPos=0;
	}

}


} // namespace Marble

Q_EXPORT_PLUGIN2( FileReaderPositionProviderPlugin, Marble::GpsSimulationPlugin )

#include "FileReaderPositionProviderPlugin.moc"

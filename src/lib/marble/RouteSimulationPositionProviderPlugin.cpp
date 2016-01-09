//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konrad Enzensberger <e.konrad@mpegcode.com>
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "RouteSimulationPositionProviderPlugin.h"

#include <QTimer>

#include "MarbleMath.h"
#include "MarbleModel.h"
#include "routing/Route.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"

namespace Marble
{

namespace {
    qreal const c_frequency = 4.0; // Hz
}

QString RouteSimulationPositionProviderPlugin::name() const
{
    return tr( "Current Route Position Provider Plugin" );
}

QString RouteSimulationPositionProviderPlugin::nameId() const
{
    return "RouteSimulationPositionProviderPlugin";
}

QString RouteSimulationPositionProviderPlugin::guiString() const
{
    return tr( "Current Route" );
}

QString RouteSimulationPositionProviderPlugin::version() const
{
    return "1.1";
}

QString RouteSimulationPositionProviderPlugin::description() const
{
    return tr( "Simulates traveling along the current route." );
}

QString RouteSimulationPositionProviderPlugin::copyrightYears() const
{
    return "2011, 2012";
}

QList<PluginAuthor> RouteSimulationPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Konrad Enzensberger", "e.konrad@mpegcode.com" )
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "nienhueser@kde.org" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" );
}

QIcon RouteSimulationPositionProviderPlugin::icon() const
{
    return QIcon();
}

PositionProviderPlugin* RouteSimulationPositionProviderPlugin::newInstance() const
{
    return new RouteSimulationPositionProviderPlugin( m_marbleModel );
}

PositionProviderStatus RouteSimulationPositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates RouteSimulationPositionProviderPlugin::position() const
{
    return m_currentPosition;
}

GeoDataAccuracy RouteSimulationPositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;

    // faked values
    result.level = GeoDataAccuracy::Detailed;
    result.horizontal = 10.0;
    result.vertical = 10.0;

    return result;
}

RouteSimulationPositionProviderPlugin::RouteSimulationPositionProviderPlugin( MarbleModel *marbleModel ) :
    PositionProviderPlugin(),
    m_marbleModel( marbleModel ),
    m_currentIndex( -2 ),
    m_status( PositionProviderStatusUnavailable ),
    m_currentDateTime(),
    m_speed( 0.0 ),
    m_direction( 0.0 )
{
    // nothing to do
}

RouteSimulationPositionProviderPlugin::~RouteSimulationPositionProviderPlugin()
{
}

void RouteSimulationPositionProviderPlugin::initialize()
{
    m_currentIndex = -1;
    m_lineString = m_lineStringInterpolated = m_marbleModel->routingManager()->routingModel()->route().path();
    m_speed = 0;   //initialize speed to be around 25 m/s;
    m_status = m_lineString.isEmpty() ? PositionProviderStatusUnavailable : PositionProviderStatusAcquiring;
    if ( !m_lineString.isEmpty() ) {
        QTimer::singleShot( 1000.0 / c_frequency, this, SLOT(update()) );
    }
}

bool RouteSimulationPositionProviderPlugin::isInitialized() const
{
    return ( m_currentIndex > -2 );
}

qreal RouteSimulationPositionProviderPlugin::speed() const
{
    return m_speed;
}

qreal RouteSimulationPositionProviderPlugin::direction() const
{
    return m_direction;
}

QDateTime RouteSimulationPositionProviderPlugin::timestamp() const
{
    return m_currentDateTime;
}

void RouteSimulationPositionProviderPlugin::update()
{
    bool isCurrentIndexValid = true;

    if ( m_currentIndex >= 0 && m_currentIndex < m_lineStringInterpolated.size() ) {
        if ( m_status != PositionProviderStatusAvailable ) {
            m_status = PositionProviderStatusAvailable;
            emit statusChanged( PositionProviderStatusAvailable );
        }

        GeoDataCoordinates newPosition = m_lineStringInterpolated.at( m_currentIndex );
        const QDateTime newDateTime = QDateTime::currentDateTime();
        qreal time= m_currentDateTime.msecsTo(newDateTime)/1000.0;
        if ( m_currentPosition.isValid() ) {
	    {//speed calculations
	     //Max speed is set on points (m_lineStringInterpolated) based on formula. (max speed before points is calculated so the acceleration won't be exceeded)
	    	const double acceleration = 1.5;
	        const double lookForwardDistance = 1000;
		double checkedDistance = distanceSphere( m_currentPosition, m_lineStringInterpolated.at(m_currentIndex) )* m_marbleModel->planetRadius();
		const double maxSpeed = 25;
		const double minSpeed = 2;
		double newSpeed = qMin((m_speed + acceleration*time), maxSpeed);
		for (int i=qMax(1,m_currentIndex); i<m_lineStringInterpolated.size()-1 && checkedDistance<lookForwardDistance; ++i)
		{
                    double previousHeading = m_lineStringInterpolated.at( i-1 ).bearing( m_lineStringInterpolated.at( i ), GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
                    double curveLength = 10;//we treat one point turn as a curve of length 10
                    double angleSum = 0;//sum of turn angles in a curve
                    for (int j=i+1; j<m_lineStringInterpolated.size() && curveLength<35; ++j)
                    {
                        double newHeading = m_lineStringInterpolated.at( j-1 ).bearing( m_lineStringInterpolated.at( j ), GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
                        double diffrenceHeading = qAbs(previousHeading-newHeading);//angle of turn
                        if(diffrenceHeading>180)
                                diffrenceHeading = 360 - diffrenceHeading;
                        angleSum +=diffrenceHeading;
                        double maxSpeedAtTurn = qMax(((1 - (angleSum/60.0/curveLength*10.0))*maxSpeed), minSpeed);//speed limit at turn
		        if( checkedDistance<25 and maxSpeedAtTurn<newSpeed )//if we are near turn don't accelerate, if we will have to slow down
			    newSpeed = qMin(newSpeed, qMax(m_speed,maxSpeedAtTurn));
                        // formulas:
                        //     s = Vc * t + a*t*t/2
                        //     V0 = Vc + a*t
                        // V0 = maxCurrentSpeed
                        // Vc = maxSpeedAtTurn
                        // s = checkedDistance
                        // a = acceleration
                        double delta = maxSpeedAtTurn*maxSpeedAtTurn - 4.0*acceleration/2.0*(-checkedDistance);//delta = b*b-4*a*c
                        double t = (-maxSpeedAtTurn+sqrt(delta))/(2.0*acceleration/2.0);//(-b+sqrt(delta))/(2*c)
                        double maxCurrentSpeed = maxSpeedAtTurn + acceleration*t;
                        newSpeed = qMin(newSpeed, maxCurrentSpeed);
                        previousHeading = newHeading;
                        curveLength += distanceSphere( m_lineStringInterpolated.at( j-1 ), m_lineStringInterpolated.at( j ) )* m_marbleModel->planetRadius();
                    }
                    checkedDistance += distanceSphere( m_lineStringInterpolated.at( i ), m_lineStringInterpolated.at( i+1 ) )* m_marbleModel->planetRadius();
		}
		m_speed=newSpeed;
	    }
	    //Assume the car's moving at m_speed m/s. The distance moved will be speed*time which is equal to the speed of the car if time is equal to one.
            //If the function isn't called once exactly after a second, multiplying by the time will compensate for the error and maintain the speed.
            double fraction = m_speed*time/(distanceSphere( m_currentPosition, newPosition )* m_marbleModel->planetRadius());

            //Interpolate and find the next point to move to if needed.
            if(fraction>0 && fraction <1){
                GeoDataCoordinates newPoint = m_currentPosition.interpolate(newPosition,fraction);
                newPosition=newPoint;
            }
            else if ( fraction > 1 ) {
                while ( fraction > 1 ) {
		    ++m_currentIndex;
		    if ( m_currentIndex >= m_lineStringInterpolated.size() ) {
                        isCurrentIndexValid = false;
                        break;
		    }

                    newPosition = m_lineStringInterpolated.at( m_currentIndex );
                    fraction = m_speed*time / (distanceSphere( m_currentPosition, newPosition )* m_marbleModel->planetRadius());
                }

                if ( isCurrentIndexValid ) {
                    GeoDataCoordinates newPoint = m_currentPosition.interpolate( newPosition, fraction );
                    newPosition = newPoint;
                }
            }
	    else
	    {
	    	m_currentIndex++;
	    }

            m_direction = m_currentPosition.bearing( newPosition, GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
        }
        m_currentPosition = newPosition;
        m_currentDateTime = newDateTime;
        emit positionChanged( position(), accuracy() );
    }
    else {
        // Repeat from start
        m_currentIndex = 0;
        m_lineStringInterpolated = m_lineString;
        m_currentPosition = GeoDataCoordinates();	//Reset The current position so that the the simulation starts from the correct starting point.
        m_speed = 0;
	if ( m_status != PositionProviderStatusUnavailable ) {
            m_status = PositionProviderStatusUnavailable;
            emit statusChanged( PositionProviderStatusUnavailable );
        }
    }

    QTimer::singleShot( 1000.0 / c_frequency, this, SLOT(update()) );
}

} // namespace Marble

#include "moc_RouteSimulationPositionProviderPlugin.cpp"

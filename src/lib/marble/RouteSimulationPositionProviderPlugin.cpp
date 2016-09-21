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

#include "MarbleMath.h"
#include "MarbleModel.h"
#include "routing/Route.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "GeoDataAccuracy.h"

#include <QIcon>

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
    return QStringLiteral("RouteSimulationPositionProviderPlugin");
}

QString RouteSimulationPositionProviderPlugin::guiString() const
{
    return tr( "Current Route" );
}

QString RouteSimulationPositionProviderPlugin::version() const
{
    return QStringLiteral("1.1");
}

QString RouteSimulationPositionProviderPlugin::description() const
{
    return tr( "Simulates traveling along the current route." );
}

QString RouteSimulationPositionProviderPlugin::copyrightYears() const
{
    return QStringLiteral("2011, 2012");
}

QVector<PluginAuthor> RouteSimulationPositionProviderPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Konrad Enzensberger"), QStringLiteral("e.konrad@mpegcode.com"))
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
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
    return m_currentPositionWithNoise;
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
    m_direction( 0.0 ),
    m_directionWithNoise(0.0)
{
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
}

RouteSimulationPositionProviderPlugin::~RouteSimulationPositionProviderPlugin()
{
}

void RouteSimulationPositionProviderPlugin::initialize()
{
    updateRoute();
    connect(m_marbleModel->routingManager()->routingModel(), SIGNAL(currentRouteChanged()), this, SLOT(updateRoute()));
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
    return m_directionWithNoise;
}

QDateTime RouteSimulationPositionProviderPlugin::timestamp() const
{
    return m_currentDateTime;
}

void RouteSimulationPositionProviderPlugin::updateRoute(){
    m_currentIndex = -1;
    m_lineString = m_lineStringInterpolated = m_marbleModel->routingManager()->routingModel()->route().path();
    m_speed = 0;   //initialize speed to be around 25 m/s;
    bool const canWork = !m_lineString.isEmpty() || m_currentPosition.isValid();
    if (canWork) {
        changeStatus(PositionProviderStatusAcquiring);
        m_updateTimer.start(1000.0 / c_frequency);
    } else {
        changeStatus(PositionProviderStatusUnavailable);
        m_updateTimer.stop();
    }
}

void RouteSimulationPositionProviderPlugin::update()
{
    if (m_lineString.isEmpty() && m_currentPosition.isValid()) {
        m_currentPositionWithNoise = addNoise(m_currentPosition, accuracy());
        changeStatus(PositionProviderStatusAvailable);
        emit positionChanged(position(), accuracy());
        return;
    }

    if ( m_currentIndex >= 0 && m_currentIndex < m_lineStringInterpolated.size() ) {
        changeStatus(PositionProviderStatusAvailable);
        GeoDataCoordinates newPosition = m_lineStringInterpolated.at( m_currentIndex );
        const QDateTime newDateTime = QDateTime::currentDateTime();
        qreal time= m_currentDateTime.msecsTo(newDateTime)/1000.0;
        if ( m_currentPosition.isValid() ) {
            //speed calculations
            //Max speed is set on points (m_lineStringInterpolated) based on formula. (max speed before points is calculated so the acceleration won't be exceeded)
            const qreal acceleration = 1.5;
            const qreal lookForwardDistance = 1000;
            qreal checkedDistance = distanceSphere( m_currentPosition, m_lineStringInterpolated.at(m_currentIndex) )* m_marbleModel->planetRadius();
            const qreal maxSpeed = 25;
            const qreal minSpeed = 2;
            qreal newSpeed = qMin((m_speed + acceleration*time), maxSpeed);
            for (int i=qMax(1,m_currentIndex); i<m_lineStringInterpolated.size()-1 && checkedDistance<lookForwardDistance; ++i)
            {
                qreal previousHeading = m_lineStringInterpolated.at( i-1 ).bearing( m_lineStringInterpolated.at( i ), GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
                qreal curveLength = 10;//we treat one point turn as a curve of length 10
                qreal angleSum = 0;//sum of turn angles in a curve
                for (int j=i+1; j<m_lineStringInterpolated.size() && curveLength<35; ++j)
                {
                    qreal newHeading = m_lineStringInterpolated.at( j-1 ).bearing( m_lineStringInterpolated.at( j ), GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
                    qreal differenceHeading = qAbs(previousHeading-newHeading);//angle of turn
                    if(differenceHeading>180) {
                        differenceHeading = 360 - differenceHeading;
                    }
                    angleSum +=differenceHeading;
                    qreal maxSpeedAtTurn = qMax((1 - (static_cast<qreal>(angleSum/60.0/curveLength*10.0))*maxSpeed), minSpeed);//speed limit at turn
                    if( checkedDistance<25 && maxSpeedAtTurn<newSpeed )//if we are near turn don't accelerate, if we will have to slow down
                        newSpeed = qMin(newSpeed, qMax(m_speed,maxSpeedAtTurn));
                    // formulas:
                    //     s = Vc * t + a*t*t/2
                    //     V0 = Vc + a*t
                    // V0 = maxCurrentSpeed
                    // Vc = maxSpeedAtTurn
                    // s = checkedDistance
                    // a = acceleration
                    qreal delta = maxSpeedAtTurn*maxSpeedAtTurn - 4.0*acceleration/2.0*(-checkedDistance);//delta = b*b-4*a*c
                    qreal t = (-maxSpeedAtTurn+sqrt(delta))/(2.0*acceleration/2.0);//(-b+sqrt(delta))/(2*c)
                    qreal maxCurrentSpeed = maxSpeedAtTurn + acceleration*t;
                    newSpeed = qMin(newSpeed, maxCurrentSpeed);
                    previousHeading = newHeading;
                    curveLength += distanceSphere( m_lineStringInterpolated.at( j-1 ), m_lineStringInterpolated.at( j ) )* m_marbleModel->planetRadius();
                }
                checkedDistance += distanceSphere( m_lineStringInterpolated.at( i ), m_lineStringInterpolated.at( i+1 ) )* m_marbleModel->planetRadius();
            }
            m_speed=newSpeed;

            //Assume the car's moving at m_speed m/s. The distance moved will be speed*time which is equal to the speed of the car if time is equal to one.
            //If the function isn't called once exactly after a second, multiplying by the time will compensate for the error and maintain the speed.
            qreal fraction = m_speed*time/(distanceSphere( m_currentPosition, newPosition )* m_marbleModel->planetRadius());

            //Interpolate and find the next point to move to if needed.
            if(fraction>0 && fraction <1){
                GeoDataCoordinates newPoint = m_currentPosition.interpolate(newPosition,fraction);
                newPosition=newPoint;
            }
            else if ( fraction > 1 ) {
                bool isCurrentIndexValid = true;
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
            m_directionWithNoise = addNoise(m_direction);
        }
        m_currentPosition = newPosition;
        m_currentPositionWithNoise = addNoise(m_currentPosition, accuracy());
        m_currentDateTime = newDateTime;
        emit positionChanged( position(), accuracy() );
    }
    else {
        // Repeat from start
        m_currentIndex = 0;
        m_lineStringInterpolated = m_lineString;
        m_currentPosition = GeoDataCoordinates();	//Reset the current position so that the simulation starts from the correct starting point.
        m_currentPositionWithNoise = GeoDataCoordinates();
        m_speed = 0;
        changeStatus(PositionProviderStatusUnavailable);
    }
}

GeoDataCoordinates RouteSimulationPositionProviderPlugin::addNoise(const Marble::GeoDataCoordinates &position, const Marble::GeoDataAccuracy &accuracy ) const
{
    qreal randomBearing = static_cast<qreal>(qrand()) / (static_cast<qreal>(RAND_MAX/M_PI));
    qreal randomDistance = static_cast<qreal>(qrand()) / (static_cast<qreal>(RAND_MAX/(accuracy.horizontal / 2.0 / m_marbleModel->planetRadius())));
    return position.moveByBearing(randomBearing, randomDistance);
}

qreal RouteSimulationPositionProviderPlugin::addNoise(qreal bearing) const
{
    qreal const maxBearingError = 30.0;
    return bearing + static_cast<qreal>(qrand()) / (static_cast<qreal>(RAND_MAX/maxBearingError/2.0)) - maxBearingError / 2.0;
}

void RouteSimulationPositionProviderPlugin::changeStatus(PositionProviderStatus status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(m_status);
    }
}

} // namespace Marble

#include "moc_RouteSimulationPositionProviderPlugin.cpp"

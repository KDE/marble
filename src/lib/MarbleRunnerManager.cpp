/*
    Copyright 2008 Henry de Valence <hdevalence@gmail.com>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "MarbleRunnerManager.h"

#include "MarblePlacemarkModel.h"
#include "PlaceMarkManager.h"
#include "PlaceMarkContainer.h"
#include "GeoDataPlacemark.h"

#include "LatLonRunner.h"
#include "OnfRunner.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include <QtDebug>

namespace Marble
{

MarbleRunnerManager::MarbleRunnerManager( QObject *parent )
            : QObject(parent)
{
    qRegisterMetaType<QVector<GeoDataPlacemark*> >("QVector<GeoDataPlacemark*>");

    m_model = new MarblePlacemarkModel(0);
    m_activeRunners = 0;
    m_lastString = "";
    
    //Runners
    m_latlonRunner = new LatLonRunner(0);
    m_latlonRunner->start();
    m_latlonRunner->moveToThread(m_latlonRunner);
    connect( m_latlonRunner, SIGNAL( runnerStarted() ),
             this,           SLOT( slotRunnerStarted() ));
    connect( m_latlonRunner, SIGNAL( runnerFinished( QVector<GeoDataPlacemark*> ) ),
             this,           SLOT( slotRunnerFinished( QVector<GeoDataPlacemark*> ) ));
    connect( this,           SIGNAL( engage(QString) ),
             m_latlonRunner, SLOT( parse(QString) ));

    m_onfRunner = new OnfRunner(0);
    m_onfRunner->start();
    m_onfRunner->moveToThread(m_onfRunner);
    connect( m_onfRunner, SIGNAL( runnerStarted() ),
             this,        SLOT( slotRunnerStarted() ));
    connect( m_onfRunner, SIGNAL( runnerFinished( QVector<GeoDataPlacemark*> ) ),
             this,        SLOT( slotRunnerFinished( QVector<GeoDataPlacemark*> ) ));
    connect( this,        SIGNAL( engage(QString) ),
             m_onfRunner, SLOT( parse(QString) ));
}

MarbleRunnerManager::~MarbleRunnerManager()
{
    m_latlonRunner->quit();
    m_latlonRunner->wait();
    delete m_latlonRunner;
    
    m_onfRunner->quit();
    m_onfRunner->wait();
    delete m_onfRunner;
}

void MarbleRunnerManager::newText(QString text)
{
    if( text != m_lastString ) {
        m_lastString = text;
        qDebug() << "Creating new model";
        MarblePlacemarkModel *model = new MarblePlacemarkModel(0);
        emit modelChanged( model );
        delete m_model;
        m_model = model;
    }
//     qDebug() << "Engage: " << text;
    emit engage(text);
}

void MarbleRunnerManager::slotRunnerFinished( QVector<GeoDataPlacemark*> result )
{
    m_activeRunners--;
    qDebug() << "Runner finished, active runners: " << m_activeRunners;
    qDebug() << "Runner reports" << result.size() << "results";
    if( result.isEmpty() )
        return;

    PlaceMarkContainer cont( result, "Runner Results" );
    m_model->addPlaceMarks( cont, false );
//     qDebug() << "emit modelchanged";
    emit modelChanged( m_model );
}

void MarbleRunnerManager::slotRunnerStarted()
{
    m_activeRunners++;
    qDebug() << "Runner starting, active runners: " << m_activeRunners;
}

MarblePlacemarkModel* MarbleRunnerManager::model()
{
    return m_model;
}

void MarbleRunnerManager::setModel( MarblePlacemarkModel *model )
{
    m_model = model;
}

}

#include "MarbleRunnerManager.moc"

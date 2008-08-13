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
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLERUNNERMANAGER_H
#define MARBLERUNNERMANAGER_H

class MarblePlacemarkModel;
class PlaceMarkManager;
//class MarbleRunnerResult;
class LatLonRunner;

#include "MarbleRunnerResult.h"

#include <QtCore/QObject>

class QString;

class MarbleRunnerManager : public QObject
{
    Q_OBJECT
public:
    MarbleRunnerManager( QObject *parent = 0 );
    ~MarbleRunnerManager();
    
    void setModel(MarblePlacemarkModel *model);
    MarblePlacemarkModel* model();
public slots:
    void newText(QString text);
signals:
    void allDone();
    void modelChanged( MarblePlacemarkModel *model );
    //tells runners to make it so
    void engage(QString text);
private slots:
    void slotRunnerFinished(MarbleRunnerResult result);
    void slotRunnerStarted();
private:
    int m_activeRunners;
    QString m_lastString;
    MarblePlacemarkModel *m_model;
    
    LatLonRunner *m_latlonRunner;
};



#endif

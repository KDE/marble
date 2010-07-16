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

#ifndef MARBLE_MARBLERUNNERMANAGER_H
#define MARBLE_MARBLERUNNERMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QMutex>

namespace Marble
{

class MarbleMap;
class MarblePlacemarkModel;
class GeoDataPlacemark;

class MarbleAbstractRunner;

class MarbleRunnerManager : public QObject
{
    Q_OBJECT
public:
    explicit MarbleRunnerManager( QObject *parent = 0 );
    ~MarbleRunnerManager();

    void setCelestialBodyId(const QString &celestialBodyId);
    void setMap(MarbleMap * map);

    /**
      * Toggle offline mode. In offline mode, runners shall not try to access
      * the network (possibly not returning any results).
      */
    void setWorkOffline( bool offline );

public slots:
    void newText(const QString& text);
signals:
    void modelChanged( MarblePlacemarkModel *model );

    void searchFinished(const QString &text);

private slots:
    void slotRunnerFinished(MarbleAbstractRunner* runner, QVector<GeoDataPlacemark*> result);

private:
    QString m_lastString;
    QMutex m_modelMutex;
    MarbleMap * m_map;
    int m_activeRunners;
    QString m_previousString;
    MarblePlacemarkModel *m_model;
    QList<MarbleAbstractRunner*> m_runners;
    QVector<GeoDataPlacemark*> m_placemarkContainer;
    QString m_celestialBodyId;
    bool m_workOffline;
};

}

#endif

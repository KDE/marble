//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

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

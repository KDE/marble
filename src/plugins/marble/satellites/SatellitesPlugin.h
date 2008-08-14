//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Claudiu Covaci <claudiu.covaci@gmail.com>
//

#ifndef SATELLITESPLUGIN_H
#define SATELLITESPLUGIN_H

#include <QtCore/QObject>

#include "MarbleAbstractLayer.h"

namespace Marble
{

/**
 * \brief This class represents the data from the NASA/NORAD Two-line-elements format.
 * See http://en.wikipedia.org/wiki/Orbital_elements#Two_line_elements
 */
class tleSatellite : public QObject
{
    Q_OBJECT
public:
    Satellite( QString firstLine, QString secondLine );
    bool isValid();
    //line 1
    int getCatalogNum();
    QChar securityClass();
    QString internationalId();
    QDateTime epochTime();
    qreal  firstTimeDeriv();
    qreal secondTimeDeriv();
    qreal bstarDrag();
    int elementNum();
    //line 2
    qreal inclination(); // in RADIANS
    qreal raAscendingNode(); // in RADIANS
    qreal eccentricity();
    qreal argPerigee();  // in RADIANS
    qreal meanAnomaly(); // in RADIANS
    qreal meanMotion(); //revs per day
    
public slots:
    update( QString firstLine, QString secondLine );
private:
    QString m_firstLine;
    QString m_secondLine;
    parse();
    
    int m_catalogNum;
    QChar m_securityClass;
    QString m_internationalId;
    QDateTime m_epochTime;
    qreal m_firstTimeDeriv;
    qreal m_secondTimeDeriv;
    qreal m_bstarDrag;
    int m_elementNum;
    
    qreal m_inclination; // in RADIANS
    qreal m_raAscendingNode; // in RADIANS
    qreal m_eccentricity;
    qreal m_argPerigee;  // in RADIANS
    qreal m_meanAnomaly; // in RADIANS
    qreal m_meanMotion; //revs per day
};
    
/**
 * \brief This class displays a layer of satellites (which satellites TBD).
 *
 */
class SatellitesPlugin : public MarbleAbstractLayer
{
    Q_OBJECT
    Q_INTERFACES( Marble::MarbleLayerInterface )

 public:
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString guiString() const;
    QString nameId() const;
    QString description() const;
    QIcon icon () const;

    void initialize ();
    bool isInitialized () const;
    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );
public slots:
    void tleDownloadComplete( QString fname, QString id );
//    QTimer* m_timer; /**< Timer to set the update interval */
private:
    HttpDownloadManager *m_dlman;
    QString 
	bool m_init;
    
};

}

#endif

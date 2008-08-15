//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

//
// This class is a stars plugin.
//

#ifndef MARBLESTARSPLUGIN_H
#define MARBLESTARSPLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "MarbleRenderPlugin.h"
#include "Quaternion.h"

class QDateTime;

namespace Marble
{

class StarPoint {
 public:
    StarPoint() {}
    /**
     * @brief create a starpoint from rectaszension and declination
     * @param  rect rectaszension
     * @param  lat declination
     * @param  mag
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    StarPoint(double rect, double decl, double mag) {
        m_q = Quaternion( rect, decl );
        m_mag = mag;
    }

    ~StarPoint(){}

    double magnitude() const {
        return m_mag;
    }

    const Quaternion &quaternion() const {
        return m_q;
    }

 private:
    double      m_mag;
    Quaternion  m_q;
};


/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class MarbleStarsPlugin : public MarbleRenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::MarbleRenderPluginInterface )

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

    // sidereal time in hours:
    double siderealTime( const QDateTime& );

 private:
    QVector<StarPoint> m_stars;
};

}

#endif // MARBLESTARSPLUGIN_H

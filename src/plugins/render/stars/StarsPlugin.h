//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

//
// This class is a stars plugin.
//

#ifndef MARBLESTARSPLUGIN_H
#define MARBLESTARSPLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "RenderPlugin.h"
#include "Quaternion.h"
#include "MarbleDebug.h"

class QDateTime;

namespace Marble
{

class StarPoint
{
 public:
    StarPoint() {}
    /**
     * @brief create a starpoint from rectaszension and declination
     * @param  rect rectaszension
     * @param  lat declination
     * @param  mag magnitude
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    StarPoint(qreal rect, qreal decl, qreal mag) {
        m_q = Quaternion::fromSpherical( rect, decl );
        m_mag = mag;
    }

    ~StarPoint(){}

    qreal magnitude() const {
        return m_mag;
    }

    const Quaternion &quaternion() const {
        return m_q;
    }

 private:
    qreal       m_mag;
    Quaternion  m_q;
};


/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class StarsPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( StarsPlugin )
 public:
    StarsPlugin();

    explicit StarsPlugin( const MarbleModel *marbleModel );

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

private Q_SLOTS:
    void requestRepaint();

 private:
    // sidereal time in hours:
    qreal siderealTime( const QDateTime& );

    void loadStars();
    bool m_renderStars;
    bool m_starsLoaded;
    QVector<StarPoint> m_stars;
    QPixmap m_pixmapSun;
};

}

#endif // MARBLESTARSPLUGIN_H

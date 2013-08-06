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
#include <QtCore/QVariant>
#include <QtCore/QHash>
#include <QtGui/QBrush>

#include "RenderPlugin.h"
#include "Quaternion.h"
#include "DialogConfigurationInterface.h"

class QDateTime;

namespace Ui
{
    class StarsConfigWidget;
}


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
    StarPoint(int id, qreal rect, qreal decl, qreal mag, int colorId) {
        m_id = id;
        m_q = Quaternion::fromSpherical( rect, decl );
        m_mag = mag;
        m_colorId = colorId;
    }

    ~StarPoint() {}

    qreal magnitude() const {
        return m_mag;
    }

    const Quaternion &quaternion() const {
        return m_q;
    }

    int id() const {
        return m_id;
    }
    
    int colorId() const {
        return m_colorId;
    } 

private:
    int         m_id;
    qreal       m_mag;
    Quaternion  m_q;
    int         m_colorId;
};

class DsoPoint
{
public:
    DsoPoint() {}
    /**
     * @brief create a dsopoint from rectaszension and declination
     * @param  rect rectaszension
     * @param  lat declination
     * @param  mag
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    DsoPoint(QString id, qreal rect, qreal decl) {
        m_id = id;
        m_q = Quaternion::fromSpherical( rect, decl );
    }

    ~DsoPoint() {}

    QString id() const {
        return m_id;
    }  

    const Quaternion &quaternion() const {
        return m_q;
    }

private:
    QString    m_id;
    Quaternion  m_q;
};

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class Constellation;

class StarsPlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN(StarsPlugin)
public:
    explicit StarsPlugin( const MarbleModel *marbleModel=0 );

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    virtual RenderType renderType() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon() const;

    void initialize();

    bool isInitialized() const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

    QDialog *configDialog();

    QHash<QString,QVariant> settings() const;

    void setSettings( const QHash<QString,QVariant> &settings );

    QString assembledConstellation(const QString &name);

protected:
    bool eventFilter( QObject *object, QEvent *e );

private Q_SLOTS:
    void requestRepaint();
    void toggleSun();
    void toggleDsos();
    void toggleDsoLabels();
    void toggleConstellationLines();
    void toggleConstellationLabels();


public Q_SLOTS:
    void readSettings();
    void writeSettings();
    void constellationGetColor();
    void constellationLabelGetColor();
    void dsoLabelGetColor();
    void eclipticGetColor();
    void celestialEquatorGetColor();
    void celestialPoleGetColor();

private:
    template<class T>
    T readSetting( const QHash<QString, QVariant> &settings, const QString &key, const T &defaultValue )
    {
        if ( !settings.contains( key ) ) {
            return defaultValue;
        }

        return qVariantValue<T>( settings[key] );
    }

    void prepareNames();
    QHash<QString, QString> m_abbrHash;
    QHash<QString, QString> m_nativeHash;
    int m_nameIndex;

    // sidereal time in hours:
    qreal siderealTime( const QDateTime& );
    void loadStars();
    void loadConstellations();
    void loadDsos();
    QDialog *m_configDialog;
    Ui::StarsConfigWidget *ui_configWidget;
    bool m_renderStars;
    bool m_renderConstellationLines;
    bool m_renderConstellationLabels;
    bool m_renderDsos;
    bool m_renderDsoLabels;
    bool m_renderSun;
    bool m_renderEcliptic;
    bool m_renderCelestialEquator;
    bool m_renderCelestialPole;
    bool m_starsLoaded;
    bool m_constellationsLoaded;
    bool m_dsosLoaded;
    QVector<StarPoint> m_stars;
    QPixmap m_pixmapSun;
    QVector<Constellation> m_constellations;
    QVector<DsoPoint> m_dsos;
    QHash<int,int> m_idHash;
    QImage m_dsoImage;
    int m_magnitudeLimit;
    QBrush m_constellationBrush;
    QBrush m_constellationLabelBrush;
    QBrush m_dsoLabelBrush;
    QBrush m_eclipticBrush;
    QBrush m_celestialEquatorBrush;
    QBrush m_celestialPoleBrush;
    QVector<QPixmap> m_pixN1Stars;
    QVector<QPixmap> m_pixP0Stars;
    QVector<QPixmap> m_pixP1Stars;
    QVector<QPixmap> m_pixP2Stars;
    QVector<QPixmap> m_pixP3Stars;
    QVector<QPixmap> m_pixP4Stars;
    QVector<QPixmap> m_pixP5Stars;
    QVector<QPixmap> m_pixP6Stars;
    QVector<QPixmap> m_pixP7Stars;

    bool m_doRender;
};

class Constellation
{
public:
    Constellation() {}
    Constellation(StarsPlugin *plug, QString &name, QString &stars) {
        m_plugin = plug;
        m_name = name;
        QStringList starlist = stars.split(" ");
        for (int i = 0; i < starlist.size(); ++i) {
            m_stars << starlist.at(i).toInt();
        }

    }

    int size() const {
        return m_stars.size();
    }

    int at(const int index) const {
        if (index < 0) return -1;
        if (index >= m_stars.size()) return -1;
        return m_stars.at(index);
    }

    QString name() const {
        return m_plugin->assembledConstellation(m_name);
    }

private:
    StarsPlugin *m_plugin;
    QString m_name;
    QVector<int> m_stars;

};

}

#endif // MARBLESTARSPLUGIN_H

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

#include <QVector>
#include <QHash>
#include <QMap>
#include <QVariant>
#include <QBrush>

#include "RenderPlugin.h"
#include "Quaternion.h"
#include "DialogConfigurationInterface.h"

class QMenu;
class QVariant;

class SolarSystem;

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
    StarPoint(int id, qreal rect, qreal decl, qreal mag, int colorId) :
        m_id( id ),
        m_magnitude( mag ),
        m_colorId( colorId )
    {
        m_q = Quaternion::fromSpherical( rect, decl );
    }

    ~StarPoint() {}

    qreal magnitude() const
    {
        return m_magnitude;
    }

    const Quaternion &quaternion() const
    {
        return m_q;
    }

    int id() const
    {
        return m_id;
    }
    
    int colorId() const
    {
        return m_colorId;
    } 

private:
    int         m_id;
    qreal       m_magnitude;
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
    DsoPoint(const QString& id, qreal rect, qreal decl) {
        m_id = id;
        m_q = Quaternion::fromSpherical( rect, decl );
    }

    QString id() const
    {
        return m_id;
    }  

    const Quaternion &quaternion() const
    {
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
    Q_PLUGIN_METADATA(IID "org.kde.marble.StarsPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN(StarsPlugin)
public:
    explicit StarsPlugin( const MarbleModel *marbleModel=0 );
    ~StarsPlugin();

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

    QVector<PluginAuthor> pluginAuthors() const override;

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
    void toggleSunMoon(bool on);
    void togglePlanets(bool on);
    void toggleDsos(bool on);
    void toggleConstellations(bool on);
    void executeConfigDialog();

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

        return settings[key].value<T>();
    }

    QPixmap starPixmap(qreal mag, int colorId) const;

    void prepareNames();
    QHash<QString, QString> m_abbrHash;
    QHash<QString, QString> m_nativeHash;
    int m_nameIndex;

    void renderPlanet(const QString &planetId,
                      GeoPainter *painter,
                      SolarSystem &sys,
                      ViewportParams *viewport,
                      qreal skyRadius,
                      matrix &skyAxisMatrix) const;
    void createStarPixmaps();
    void loadStars();
    void loadConstellations();
    void loadDsos();
    QPointer<QDialog> m_configDialog;
    Ui::StarsConfigWidget *ui_configWidget;
    bool m_renderStars;
    bool m_renderConstellationLines;
    bool m_renderConstellationLabels;
    bool m_renderDsos;
    bool m_renderDsoLabels;
    bool m_renderSun;
    bool m_renderMoon;
    QMap<QString, bool> m_renderPlanet;
    bool m_renderEcliptic;
    bool m_renderCelestialEquator;
    bool m_renderCelestialPole;
    bool m_starsLoaded;
    bool m_starPixmapsCreated;
    bool m_constellationsLoaded;
    bool m_dsosLoaded;
    bool m_zoomSunMoon;
    bool m_viewSolarSystemLabel;
    QVector<StarPoint> m_stars;
    QPixmap m_pixmapSun;
    QPixmap m_pixmapMoon;
    QVector<Constellation> m_constellations;
    QVector<DsoPoint> m_dsos;
    QHash<int,int> m_idHash;
    QImage m_dsoImage;
    int m_magnitudeLimit;
    int m_zoomCoefficient;
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

    /* Context menu */
    QPointer<QMenu> m_contextMenu;
    QAction* m_constellationsAction;
    QAction* m_sunMoonAction;
    QAction* m_planetsAction;
    QAction* m_dsoAction;

    bool m_doRender;
};

class Constellation
{
public:
    Constellation() {}
    Constellation(StarsPlugin *plugin, const QString &name, const QString &stars) :
        m_plugin( plugin ),
        m_name( name )
    {
        const QStringList starlist = stars.split(QLatin1Char(' '));
        for (int i = 0; i < starlist.size(); ++i) {
            m_stars << starlist.at(i).toInt();
        }

    }

    int size() const
    {
        return m_stars.size();
    }

    int at(const int index) const
    {
        if (index < 0) {
            return -1;
        }
        if (index >= m_stars.size()) {
            return -1;
        }
        return m_stars.at(index);
    }

    QString name() const
    {
        return m_plugin->assembledConstellation(m_name);
    }

private:
    StarsPlugin *m_plugin;
    QString m_name;
    QVector<int> m_stars;

};

}

#endif // MARBLESTARSPLUGIN_H

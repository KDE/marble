// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
//

//
// This class is a stars plugin.
//

#ifndef MARBLESTARSPLUGIN_H
#define MARBLESTARSPLUGIN_H

#include <QBrush>
#include <QHash>
#include <QList>
#include <QMap>
#include <QVariant>

#include "DialogConfigurationInterface.h"
#include "Quaternion.h"
#include "RenderPlugin.h"

class QMenu;

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
    StarPoint() = default;
    /**
     * @brief create a starpoint from right ascension and declination
     * @param  id identifier
     * @param  rect right ascension
     * @param  decl declination
     * @param  mag magnitude
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     * @param  colorId color
     */
    StarPoint(int id, qreal rect, qreal decl, qreal mag, int colorId)
        : m_id(id)
        , m_magnitude(mag)
        , m_colorId(colorId)
    {
        m_q = Quaternion::fromSpherical(rect, decl);
    }

    ~StarPoint() = default;

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
    int m_id;
    qreal m_magnitude;
    Quaternion m_q;
    int m_colorId;
};

class DsoPoint
{
public:
    DsoPoint() = default;
    /**
     * @brief create a dsopoint from right ascension and declination
     * @param  id point identifier
     * @param  rect right ascension
     * @param  decl declination
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    DsoPoint(const QString &id, qreal rect, qreal decl)
    {
        m_id = id;
        m_q = Quaternion::fromSpherical(rect, decl);
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
    QString m_id;
    Quaternion m_q;
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
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(StarsPlugin)
public:
    explicit StarsPlugin(const MarbleModel *marbleModel = nullptr);
    ~StarsPlugin() override;

    QStringList backendTypes() const override;

    QString renderPolicy() const override;

    QStringList renderPosition() const override;

    RenderType renderType() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    void initialize() override;

    bool isInitialized() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = nullptr) override;

    QDialog *configDialog() override;

    QHash<QString, QVariant> settings() const override;

    void setSettings(const QHash<QString, QVariant> &settings) override;

    QString assembledConstellation(const QString &name);

protected:
    bool eventFilter(QObject *object, QEvent *e) override;

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
    T readSetting(const QHash<QString, QVariant> &settings, const QString &key, const T &defaultValue)
    {
        if (!settings.contains(key)) {
            return defaultValue;
        }

        return settings[key].value<T>();
    }

    QPixmap starPixmap(qreal mag, int colorId) const;

    void prepareNames();
    QHash<QString, QString> m_abbrHash;
    QHash<QString, QString> m_nativeHash;
    int m_nameIndex;

    void renderPlanet(const QString &planetId, GeoPainter *painter, SolarSystem &sys, ViewportParams *viewport, qreal skyRadius, matrix &skyAxisMatrix) const;
    void createStarPixmaps();
    void loadStars();
    void loadConstellations();
    void loadDsos();
    QPointer<QDialog> m_configDialog;
    Ui::StarsConfigWidget *ui_configWidget = nullptr;
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
    QList<StarPoint> m_stars;
    QPixmap m_pixmapSun;
    QPixmap m_pixmapMoon;
    QList<Constellation> m_constellations;
    QList<DsoPoint> m_dsos;
    QHash<int, int> m_idHash;
    QImage m_dsoImage;
    int m_magnitudeLimit;
    int m_zoomCoefficient;
    QBrush m_constellationBrush;
    QBrush m_constellationLabelBrush;
    QBrush m_dsoLabelBrush;
    QBrush m_eclipticBrush;
    QBrush m_celestialEquatorBrush;
    QBrush m_celestialPoleBrush;
    QList<QPixmap> m_pixN1Stars;
    QList<QPixmap> m_pixP0Stars;
    QList<QPixmap> m_pixP1Stars;
    QList<QPixmap> m_pixP2Stars;
    QList<QPixmap> m_pixP3Stars;
    QList<QPixmap> m_pixP4Stars;
    QList<QPixmap> m_pixP5Stars;
    QList<QPixmap> m_pixP6Stars;
    QList<QPixmap> m_pixP7Stars;

    /* Context menu */
    QPointer<QMenu> m_contextMenu;
    QAction *m_constellationsAction = nullptr;
    QAction *m_sunMoonAction = nullptr;
    QAction *m_planetsAction = nullptr;
    QAction *m_dsoAction = nullptr;

    bool m_doRender;
};

class Constellation
{
public:
    Constellation() = default;
    Constellation(StarsPlugin *plugin, const QString &name, const QString &stars)
        : m_plugin(plugin)
        , m_name(name)
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
    StarsPlugin *m_plugin = nullptr;
    QString m_name;
    QList<int> m_stars;
};

}

#endif // MARBLESTARSPLUGIN_H

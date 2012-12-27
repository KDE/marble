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
    StarPoint(int id, qreal rect, qreal decl, qreal mag, QColor color) {
        m_id = id;
        m_q = Quaternion::fromSpherical( rect, decl );
        m_mag = mag;
        m_color = color;
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
    
    QColor color() const {
        return m_color;
    }

private:
    int         m_id;
    qreal       m_mag;
    Quaternion  m_q;
    QColor      m_color;
};


class Constellation
{
public:
    Constellation() {}
    Constellation(QString &name, QString &stars) {
        m_name = name;
        QStringList starlist = stars.split(" ");
        for (int i = 0; i < starlist.size(); i++) {
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
        return m_name;
    }

private:
    QString m_name;
    QVector<int> m_stars;

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
    DsoPoint(qreal rect, qreal decl) {
        m_q = Quaternion::fromSpherical( rect, decl );
    }

    ~DsoPoint() {}

    const Quaternion &quaternion() const {
        return m_q;
    }

private:
    Quaternion  m_q;
};

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

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

protected:
    bool eventFilter( QObject *object, QEvent *e );

private Q_SLOTS:
    void requestRepaint();
    void toggleSun();
    void toggleDsos();
    void toggleConstellationLines();
    void toggleConstellationLabels();

public Q_SLOTS:
    void readSettings();
    void writeSettings();
    void constellationGetColor();

private:
    template<class T>
    T readSetting( const QHash<QString, QVariant> &settings, const QString &key, const T &defaultValue )
    {
        if ( !settings.contains( key ) ) {
            return defaultValue;
        }

        return qVariantValue<T>( settings[key] );
    }

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
    bool m_renderSun;
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
};

}

#endif // MARBLESTARSPLUGIN_H

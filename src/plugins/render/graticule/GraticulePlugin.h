//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>
//

//
// This class is a graticule plugin.
//

#ifndef MARBLEGRATICULEPLUGIN_H
#define MARBLEGRATICULEPLUGIN_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtGui/QPen>
#include <QtGui/QIcon>
#include <QtGui/QColorDialog>
#include <QtGui/QAbstractButton>


#include "AbstractDataPlugin.h"
#include "RenderPlugin.h"
#include "RenderPluginInterface.h"


#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"


namespace Ui 
{
    class GraticuleConfigWidget;
}

namespace Marble
{

class GeoDataLatLonAltBox;

/**
 * @brief A plugin that creates a coordinate grid on top of the map.
 * Unlike in all other classes we are using degree by default in this class.
 * This choice was made due to the fact that all common coordinate grids focus fully 
 * on the degree system. 
 */

class PluginAboutDialog;

class GraticulePlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( GraticulePlugin )

 public:
    GraticulePlugin();
    
    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    QDialog *configDialog();

    QDialog *aboutDialog();

    void initialize ();

    bool isInitialized () const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

//    QHash<QString,QVariant> settings() const;

//    void setSettings( QHash<QString,QVariant> settings );

    virtual QHash<QString,QVariant> settings() const;

    virtual void setSettings( QHash<QString,QVariant> settings );



 public Q_SLOTS:
    void readSettings();
    void writeSettings();
    
    void gridGetColor();
    void tropicsGetColor();
    void equatorGetColor();
    
    void updateSettings();


 private:
     /**
     * @brief Renders the coordinate grid within the defined view bounding box.
     * @param painter the painter used to draw the grid
     * @param viewport the viewport
     */
    void renderGrid( GeoPainter *painter, ViewportParams *viewport,
                     const QPen& equatorCirclePen,    
                     const QPen& tropicsCirclePen,
                     const QPen& gridCirclePen );

     /**
     * @brief Renders a latitude line within the defined view bounding box.
     * @param painter the painter used to draw the latitude line
     * @param latitude the latitude of the coordinate line measured in degree .
     * @param viewLatLonAltBox the latitude longitude bounding box that is covered by the view.
     */
    void renderLatitudeLine(  GeoPainter *painter, qreal latitude,
                              const GeoDataLatLonAltBox& viewLatLonAltBox = GeoDataLatLonAltBox(),
                              const QString& lineLabel = QString(), 
                              LabelPositionFlags labelPositionFlags = LineCenter );

    /**
     * @brief Renders a longitude line within the defined view bounding box.
     * @param painter the painter used to draw the latitude line
     * @param longitude the longitude of the coordinate line measured in degree .
     * @param viewLatLonAltBox the latitude longitude bounding box that is covered by the view.
     * @param polarGap the area around the poles in which most longitude lines are not drawn
     *        for reasons of aesthetics and clarity of the map. The polarGap avoids narrow
     *        concurring lines around the poles which obstruct the view onto the surface.
     *        The radius of the polarGap area is measured in degrees. 
     * @param lineLabel draws a label using the font and color properties set for the painter.
     */
    void renderLongitudeLine( GeoPainter *painter, qreal longitude,                         
                              const GeoDataLatLonAltBox& viewLatLonAltBox = GeoDataLatLonAltBox(),
                              qreal northPolarGap = 0.0, qreal southPolarGap = 0.0,
                              const QString& lineLabel = QString(),
                              LabelPositionFlags labelPositionFlags = LineCenter );

    /**
     * @brief Renders the latitude lines that are visible within the defined view bounding box.
     * @param painter the painter used to draw the latitude lines
     * @param viewLatLonAltBox the latitude longitude bounding box that is covered by the view.
     * @param step the angular distance between the lines measured in degrees .
     */
    void renderLatitudeLines( GeoPainter *painter, 
                              const GeoDataLatLonAltBox& viewLatLonAltBox,
                              qreal step,
                              LabelPositionFlags labelPositionFlags = LineCenter
                            );

    /**
     * @brief Renders the latitude lines that are visible within the defined view bounding box.
     * @param painter the painter used to draw the latitude lines
     * @param viewLatLonAltBox the latitude longitude bounding box that is covered by the view.
     * @param step the angular distance between the lines measured in degrees .
     * @param polarGap the area around the poles in which most longitude lines are not drawn
     *        for reasons of aesthetics and clarity of the map. The polarGap avoids narrow
     *        concurring lines around the poles which obstruct the view onto the surface.
     *        The radius of the polarGap area is measured in degrees. 
     */
    void renderLongitudeLines( GeoPainter *painter, 
                              const GeoDataLatLonAltBox& viewLatLonAltBox, 
                              qreal step, 
                              qreal northPolarGap = 0.0, qreal southPolarGap = 0.0,
                              LabelPositionFlags labelPositionFlags = LineCenter
                             );

    /**
     * @brief Maps the number of coordinate lines per 360 deg against the globe radius on the screen.
     * @param notation Determines whether the graticule is according to the DMS or Decimal system.
     */
    void initLineMaps( GeoDataCoordinates::Notation notation );

    GeoDataCoordinates::Notation m_currentNotation;

    // Maps the zoom factor to the amount of lines per 360 deg
    QMap<qreal,qreal> m_boldLineMap;
    QMap<qreal,qreal> m_normalLineMap;

    QPen m_equatorCirclePen;
    QPen m_tropicsCirclePen;
    QPen m_gridCirclePen;
    QPen m_shadowPen;
    
    QColor m_gridColor, m_tropicsColor, m_equatorColor;

    bool m_isInitialized;

    QHash<QString,QVariant> m_settings;

    QIcon m_icon;
    PluginAboutDialog *m_aboutDialog;
        
    Ui::GraticuleConfigWidget *ui_configWidget;
    QDialog *m_configDialog;
};

}

#endif // MARBLEGRATICULEPLUGIN_H

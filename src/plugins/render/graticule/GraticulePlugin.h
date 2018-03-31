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

#include <QMap>
#include <QHash>
#include <QPen>
#include <QIcon>
#include <QColorDialog>


#include "DialogConfigurationInterface.h"
#include "RenderPlugin.h"

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

class GraticulePlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GraticulePlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( GraticulePlugin )

 public:
    GraticulePlugin();

    explicit GraticulePlugin( const MarbleModel *marbleModel );

    QStringList backendTypes() const override;

    QString renderPolicy() const override;

    QStringList renderPosition() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon () const override;

    QDialog *configDialog() override;

    void initialize () override;

    bool isInitialized () const override;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = nullptr ) override;

    qreal zValue() const override;

    QHash<QString,QVariant> settings() const override;

    void setSettings( const QHash<QString,QVariant> &settings ) override;



 public Q_SLOTS:
    void readSettings();
    void writeSettings();

    void gridGetColor();
    void tropicsGetColor();
    void equatorGetColor();


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
    static void renderLatitudeLine( GeoPainter *painter, qreal latitude,
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
    static void renderLongitudeLine( GeoPainter *painter, qreal longitude,
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
                              qreal step, qreal skipStep,
                              LabelPositionFlags labelPositionFlags = LineCenter
                            );

    /**
     * @brief Renders the longitude lines that are visible within the defined view bounding box.
     * @param painter the painter used to draw the latitude lines
     * @param viewLatLonAltBox the latitude longitude bounding box that is covered by the view.
     * @param step the angular distance between the lines measured in degrees .
     * @param northPolarGap the area around the north pole in which most longitude lines are not drawn
     *        for reasons of aesthetics and clarity of the map. The polarGap avoids narrow
     *        concurring lines around the poles which obstruct the view onto the surface.
     *        The radius of the polarGap area is measured in degrees.
     * @param southPolarGap the area around the south pole in which most longitude lines are not drawn
     *        for reasons of aesthetics and clarity of the map. The polarGap avoids narrow
     *        concurring lines around the poles which obstruct the view onto the surface.
     *        The radius of the polarGap area is measured in degrees. 
     */
    void renderLongitudeLines( GeoPainter *painter, 
                              const GeoDataLatLonAltBox& viewLatLonAltBox, 
                              qreal step, qreal skipStep,
                              qreal northPolarGap = 0.0, qreal southPolarGap = 0.0,
                              LabelPositionFlags labelPositionFlags = LineCenter
                             ) const;

    /**
     * @brief Renders UTM exceptions that are visible within the defined view bounding box.
     * @param painter the painter used to draw the latitude lines
     * @param viewLatLonAltBox the latitude longitude bounding box that is covered by the view.
     * @param step the angular distance between the lines measured in degrees .
     * @param northPolarGap the area around the north pole in which most longitude lines are not drawn
     *        for reasons of aesthetics and clarity of the map. The polarGap avoids narrow
     *        concurring lines around the poles which obstruct the view onto the surface.
     *        The radius of the polarGap area is measured in degrees.
     * @param southPolarGap the area around the south pole in which most longitude lines are not drawn
     *        for reasons of aesthetics and clarity of the map. The polarGap avoids narrow
     *        concurring lines around the poles which obstruct the view onto the surface.
     *        The radius of the polarGap area is measured in degrees.
     */
    static void renderUtmExceptions( GeoPainter *painter,
                                     const GeoDataLatLonAltBox& viewLatLonAltBox,
                                     qreal step,
                                     qreal northPolarGap, qreal southPolarGap,
                                     const QString & label,
                                     LabelPositionFlags labelPositionFlags );

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
    bool m_showPrimaryLabels;
    bool m_showSecondaryLabels;

    bool m_isInitialized;

    QIcon m_icon;

    Ui::GraticuleConfigWidget *ui_configWidget;
    QDialog *m_configDialog;
};

}

#endif // MARBLEGRATICULEPLUGIN_H

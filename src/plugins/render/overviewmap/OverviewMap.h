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
// This class is a test plugin.
//

#ifndef MARBLEOVERVIEWMAP_H
#define MARBLEOVERVIEWMAP_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtGui/QColor>
#include <QtGui/QAbstractButton>
#include <QtSvg/QSvgWidget>
#include <QtSvg/QSvgRenderer>

#include "GeoDataLatLonAltBox.h"
#include "AbstractFloatItem.h"

namespace Ui
{
    class OverviewMapConfigWidget;
}

namespace Marble
{

/**
 * @short The class that creates an overview map.
 *
 */

class OverviewMap : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( OverviewMap )
    
 public:
    explicit OverviewMap( const QPointF &point = QPointF( 10.5, 10.5 ),
                          const QSizeF &size = QSizeF( 166.0, 86.0 ) );
    ~OverviewMap();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    QDialog *configDialog() const;

    void initialize ();

    bool isInitialized () const;

    void changeViewport( ViewportParams *viewport );

    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer = 0 );

    /**
     * @return: The settings of the item.
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * Set the settings of the item.
     */
    virtual void setSettings( QHash<QString,QVariant> settings );

 public slots:
    void readSettings() const;
    void writeSettings();
    void updateSettings();

 protected:
    bool eventFilter( QObject *object, QEvent *e );

 private:
    void changeBackground( const QString& target ) const;
    QSvgWidget *currentWidget() const;
    void setCurrentWidget( QSvgWidget *widget ) const;
    void loadPlanetMaps() const;
    void loadMapSuggestions() const;

    QString m_target;
    mutable QSvgRenderer  m_svgobj;
    mutable QHash<QString, QSvgWidget *> m_svgWidgets;
    mutable QHash<QString, QString> m_svgPaths;
    mutable QStringList    m_planetID;
    QPixmap        m_worldmap;
    QHash<QString,QVariant> m_settings;
    QColor m_posColor;
    QSizeF m_defaultSize;

    mutable Ui::OverviewMapConfigWidget *ui_configWidget;
    mutable QDialog *m_configDialog;

    GeoDataLatLonAltBox m_latLonAltBox;
    qreal m_centerLat;
    qreal m_centerLon;
    mutable bool m_mapChanged;

 private slots:
    void chooseCustomMap();
    void synchronizeSpinboxes();
    void showCurrentPlanetPreview() const;
    void choosePositionIndicatorColor();
    void useMapSuggestion( int index );
    void evaluateClickedButton( QAbstractButton *button );
};

}

#endif

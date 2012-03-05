//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
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

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon () const;

    QDialog *configDialog();

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
    void readSettings();
    void writeSettings();
    void updateSettings();

 protected:
    bool eventFilter( QObject *object, QEvent *e );

 private:
    void changeBackground( const QString& target );
    QSvgWidget *currentWidget() const;
    void setCurrentWidget( QSvgWidget *widget );
    void loadPlanetMaps();
    void loadMapSuggestions();

    QString m_target;
    QSvgRenderer   m_svgobj;
    QHash<QString, QSvgWidget *> m_svgWidgets;
    QHash<QString, QString> m_svgPaths;
    QStringList    m_planetID;
    QPixmap        m_worldmap;
    QHash<QString,QVariant> m_settings;
    QColor m_posColor;
    QSizeF m_defaultSize;

    Ui::OverviewMapConfigWidget *ui_configWidget;
    QDialog *m_configDialog;

    GeoDataLatLonAltBox m_latLonAltBox;
    qreal m_centerLat;
    qreal m_centerLon;
    bool m_mapChanged;

 private slots:
    void chooseCustomMap();
    void synchronizeSpinboxes();
    void showCurrentPlanetPreview() const;
    void choosePositionIndicatorColor();
    void useMapSuggestion( int index );
};

}

#endif

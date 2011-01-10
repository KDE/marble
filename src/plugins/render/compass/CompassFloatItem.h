//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#ifndef COMPASS_FLOAT_ITEM_H
#define COMPASS_FLOAT_ITEM_H

#include <QtCore/QObject>

#include "AbstractFloatItem.h"
#include "PluginAboutDialog.h"

class QSvgRenderer;

namespace Ui {
    class CompassConfigWidget;
}

namespace Marble
{

/**
 * @short The class that creates a compass
 *
 */

class CompassFloatItem  : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( CompassFloatItem )
 public:
    explicit CompassFloatItem ( const QPointF &point = QPointF( -1.0, 10.0 ),
                                const QSizeF &size = QSizeF( 75.0, 75.0 ) );
    ~CompassFloatItem ();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    QDialog *aboutDialog();
    
    void initialize ();

    bool isInitialized () const;

    QPainterPath backgroundShape() const;

    void changeViewport( ViewportParams *viewport );

    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer = 0 );

    QDialog *configDialog();

    QHash<QString,QVariant> settings() const;

    void setSettings( QHash<QString,QVariant> settings );

private Q_SLOTS:
   void readSettings() const;

   void writeSettings();

 private:
    Q_DISABLE_COPY( CompassFloatItem )

    bool           m_isInitialized;

    mutable PluginAboutDialog *m_aboutDialog;
    mutable QSvgRenderer  *m_svgobj;
    mutable QPixmap        m_compass;

    /// allowed values: -1, 0, 1; default here: 0. FIXME: Declare enum
    int            m_polarity;

    QHash<QString,QVariant> m_settings;
    /** @todo: Refactor plugin interface to have configDialog() non-const */
    mutable QDialog * m_configDialog;
    mutable Ui::CompassConfigWidget * m_uiConfigWidget;
};

}

#endif

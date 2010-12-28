//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#ifndef MAPSCALEFLOATITEM_H
#define MAPSCALEFLOATITEM_H

#include <QtCore/QObject>

#include "AbstractFloatItem.h"
#include "PluginAboutDialog.h"

namespace Ui
{
    class MapScaleConfigWidget;
}

namespace Marble
{

class PluginAboutDialog;

/**
 * @short The class that creates a map scale.
 *
 */

class MapScaleFloatItem : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( MapScaleFloatItem )
 public:
    explicit MapScaleFloatItem( const QPointF &point = QPointF( 10.5, -10.5 ),
                                const QSizeF &size = QSizeF( 0.0, 40.0 ) );
    ~MapScaleFloatItem();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    QDialog *aboutDialog() const;


    void initialize ();

    bool isInitialized () const;

    void changeViewport( ViewportParams *viewport );

    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer = 0 );


    QDialog *configDialog() const;

 protected:
    virtual bool eventFilter( QObject *object, QEvent *e );

 private Q_SLOTS:
    void readSettings() const;
    void writeSettings();
    void toggleRatioScaleVisibility();

 private:
    int   invScale() const            { return m_invScale; }
    void  setInvScale( int invScale ) { m_invScale = invScale; }

    mutable PluginAboutDialog *m_aboutDialog;
    mutable QDialog *m_configDialog;
    mutable Ui::MapScaleConfigWidget *ui_configWidget;

    int      m_radius;
    int      m_invScale;

    QString  m_target;

    int      m_leftBarMargin;
    int      m_rightBarMargin;
    int      m_scaleBarWidth;
    int      m_viewportWidth;
    int      m_scaleBarHeight;
    qreal    m_scaleBarDistance;

    int      m_bestDivisor;
    int      m_pixelInterval;
    int      m_valueInterval;

    QString  m_unit;
    QString m_ratioString;

    bool     m_scaleInitDone;

    bool     m_showRatioScale;

    void calcScaleBar();
};

}

#endif // MAPSCALEFLOATITEM_H

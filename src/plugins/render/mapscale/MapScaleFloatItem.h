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
#include "DialogConfigurationInterface.h"

namespace Ui
{
    class MapScaleConfigWidget;
}

namespace Marble
{

/**
 * @short The class that creates a map scale.
 *
 */

class MapScaleFloatItem : public AbstractFloatItem, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( MapScaleFloatItem )
 public:
    MapScaleFloatItem();
    explicit MapScaleFloatItem( const MarbleModel *marbleModel );
    ~MapScaleFloatItem();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    void changeViewport( ViewportParams *viewport );

    void paintContent( QPainter *painter );


    QDialog *configDialog();

 protected:
    virtual void contextMenuEvent( QWidget *w, QContextMenuEvent *e );
    virtual void toolTipEvent( QHelpEvent *e );

 private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void toggleRatioScaleVisibility();

 private:
    QDialog *m_configDialog;
    Ui::MapScaleConfigWidget *ui_configWidget;

    int      m_radius;

    QString  m_target;

    int      m_leftBarMargin;
    int      m_rightBarMargin;
    int      m_scaleBarWidth;
    int      m_viewportWidth;
    int      m_scaleBarHeight;
    qreal    m_scaleBarDistance;

    qreal    m_pixel2Length;
    int      m_bestDivisor;
    int      m_pixelInterval;
    int      m_valueInterval;

    QString  m_unit;
    QString m_ratioString;

    bool     m_scaleInitDone;

    bool     m_showRatioScale;

    QMenu*   m_contextMenu;

    void calcScaleBar();
};

}

#endif // MAPSCALEFLOATITEM_H

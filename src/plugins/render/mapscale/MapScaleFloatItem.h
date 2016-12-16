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
    Q_PLUGIN_METADATA(IID "org.kde.marble.MapScaleFloatItem")
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( MapScaleFloatItem )
 public:
    explicit MapScaleFloatItem( const MarbleModel *marbleModel = 0 );
    ~MapScaleFloatItem() override;

    QStringList backendTypes() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon () const override;

    void initialize () override;

    bool isInitialized () const override;

    void setProjection( const ViewportParams *viewport ) override;

    void paintContent( QPainter *painter ) override;


    QDialog *configDialog() override;

 protected:
    void contextMenuEvent( QWidget *w, QContextMenuEvent *e ) override;
    void toolTipEvent( QHelpEvent *e ) override;

 private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void toggleRatioScaleVisibility();
    void toggleMinimized();

private:
    void calcScaleBar();

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

    QString m_ratioString;

    bool     m_scaleInitDone;

    bool     m_showRatioScale;

    QMenu*   m_contextMenu;

    QAction  *m_minimizeAction;
    bool m_minimized;
    int m_widthScaleFactor;
};

}

#endif // MAPSCALEFLOATITEM_H

//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_ALTITUDEPROFILE_H
#define MARBLE_ALTITUDEPROFILE_H

#include "AbstractFloatItem.h"

class QLabel;
class KPlotObject;

namespace Marble {

class MarbleWidget;
class WidgetGraphicsItem;
class GeoDataDocument;
class TileLoader;
class PlotWidget;

class AltitudeProfile : public Marble::AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( AltitudeProfile )
    
public:
    explicit AltitudeProfile(const QPointF& point = QPointF( 10.0, 10.0 ), const QSizeF& size = QSizeF( 150.0, 50.0 ));
    virtual QStringList backendTypes() const;
    virtual bool isInitialized() const;
    virtual void initialize();
    virtual QIcon icon() const;
    virtual QString description() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString name() const;

    void forceUpdate();

protected:
    bool eventFilter(QObject *object, QEvent *e);

private slots:
    void currentRouteChanged( GeoDataDocument* );
    void altitudeDataLoadCompleted();

private:
    bool m_isInitialized;
    TileLoader *m_tileLoader;

    MarbleWidget *m_marbleWidget;

    /** The GraphicsItem presenting the widgets. NavigationFloatItem doesn't take direct ownership
        of this */
    WidgetGraphicsItem *m_widgetItem;

    PlotWidget *m_graph;
    KPlotObject *m_plot;
    QLabel *m_stats;
};

}

#endif // MARBLE_ALTITUDEPROFILE_H

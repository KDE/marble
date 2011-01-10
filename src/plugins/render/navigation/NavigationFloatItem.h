//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef NAVIGATION_FLOAT_ITEM_H
#define NAVIGATION_FLOAT_ITEM_H

#include <QtCore/QObject>

#include "global.h"

#include "AbstractFloatItem.h"

namespace Ui
{
    class Navigation;
    class NavigationSmall;
}

namespace Marble
{

class MarbleWidget;
class WidgetGraphicsItem;

/**
 * @short Provides a float item with zoom and move controls
 *
 */
class NavigationFloatItem: public AbstractFloatItem
{
    Q_OBJECT

    Q_INTERFACES( Marble::RenderPluginInterface )

MARBLE_PLUGIN( NavigationFloatItem )

 public:
    explicit NavigationFloatItem( const QPointF &point = QPointF( -10, -10 ) );
    ~NavigationFloatItem();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    void changeViewport( ViewportParams *viewport );

 protected:
    bool eventFilter( QObject *object, QEvent *e );

 private Q_SLOTS:
    /** Adjust slider value to zoom level provided */
    void setZoomSliderValue( int level );

    void setMarbleZoomValue( int level );

    /** Map theme was changed, adjust controls */
    void selectTheme( QString theme );

    /** Decrease quality during zooming */
    void adjustForAnimation();

    /** Normal quality when not zooming */
    void adjustForStill();

    /** Enable/disable zoom in/out buttons */
    void updateButtons( int zoomValue );

 private:
    /** MarbleWidget this float item is installed as event filter for */
    MarbleWidget *m_marbleWidget;

    /** The GraphicsItem presenting the widgets. NavigationFloatItem doesn't take direct ownership
        of this */
    WidgetGraphicsItem *m_widgetItem;

    /** Navigation controls */
    Ui::NavigationSmall *m_navigationWidgetSmall;
    Ui::Navigation *m_navigationWidget;

    /** Used Profile */
    MarbleGlobal::Profiles m_profiles;

    /** Radius of the viewport last time */
    int m_oldViewportRadius;
};

}

#endif // NAVIGATION_FLOAT_ITEM_H

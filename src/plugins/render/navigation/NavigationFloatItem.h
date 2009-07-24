//
// This file is part of the Marble Desktop Globe.
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

// forward declarations
class QSlider;

#include "AbstractFloatItem.h"

#include "ui_navigation.h"

namespace Marble
{

class MarbleWidget;

/**
 * @short Provides a float item with zoom and move controls
 *
 */
class NavigationFloatItem: public AbstractFloatItem {
Q_OBJECT
//Q_INTERFACES( MarbleLayerInterface )
MARBLE_PLUGIN( NavigationFloatItem )
 public:
    explicit NavigationFloatItem( const QPointF &point = QPointF( -10, -10 ),
                            const QSizeF &size = QSizeF( 100.0, 250.0 ) );
    ~NavigationFloatItem();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    virtual QPainterPath backgroundShape() const;

    void changeViewport( ViewportParams *viewport );

    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer *layer = 0 );

 protected:
    bool eventFilter( QObject *object, QEvent *e );

 private Q_SLOTS:
    /** Adjust slider value to zoom level provided */
    void zoomChanged(int level);

    /** Map theme was changed, adjust controls */
    void selectTheme( QString theme );

    /** Decrease quality during zooming */
    void adjustForAnimation();

    /** Normal quality when not zooming */
    void adjustForStill();

    /** Enable/disable zoom in/out buttons */
    void updateButtons( int value );

 private:
    /** MarbleWidget this float item is installed as event filter for */
    MarbleWidget *m_marbleWidget;

    /** Navigation controls */
    Ui::Navigation m_navigationWidget;

    /** Navigation controls embedding widget */
    QWidget *m_navigationParent;

    /** Radius of the viewport last time */
    int m_oldViewportRadius;
};

}

#endif // NAVIGATION_FLOAT_ITEM_H

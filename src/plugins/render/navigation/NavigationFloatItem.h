//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2013 Mohammed Nafees  <nafees.technocool@gmail.com>
//

#ifndef NAVIGATION_FLOAT_ITEM_H
#define NAVIGATION_FLOAT_ITEM_H

#include <QtCore/QObject>
#include <QMenu>

#include "MarbleGlobal.h"

#include "AbstractFloatItem.h"

namespace Ui
{
    class Navigation;
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
    NavigationFloatItem();
    explicit NavigationFloatItem( const MarbleModel *marbleModel );
    ~NavigationFloatItem();

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

    QPixmap pixmap( const QString &Id );

 protected:
    bool eventFilter( QObject *object, QEvent *e );
    void paintContent( QPainter *painter );
    void contextMenuEvent( QWidget *w, QContextMenuEvent *e );

 private Q_SLOTS:
    /** Map theme was changed, adjust controls */
    void selectTheme( QString theme );

    /** Enable/disable zoom in/out buttons */
    void updateButtons( int zoomValue );

    void writeSettings();
    void toggleToCurrentPositionButton();
    void toggleToHomeButton();
    void centerOnCurrentLocation();

 private:
    /** MarbleWidget this float item is installed as event filter for */
    MarbleWidget *m_marbleWidget;

    /** The GraphicsItem presenting the widgets. NavigationFloatItem doesn't take direct ownership
        of this */
    WidgetGraphicsItem *m_widgetItem;

    /** Navigation controls */
    Ui::Navigation *m_navigationWidget;

    /** Used Profile */
    MarbleGlobal::Profiles m_profiles;

    /** Radius of the viewport last time */
    int m_oldViewportRadius;

    int m_maxZoom;
    int m_minZoom;

    QMenu *m_contextMenu;
    QAction *m_activateCurrentPositionButtonAction;
    QAction *m_activateHomeButtonAction;
};

}

#endif // NAVIGATION_FLOAT_ITEM_H

//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2013 Mohammed Nafees  <nafees.technocool@gmail.com>
//

#ifndef NAVIGATION_FLOAT_ITEM_H
#define NAVIGATION_FLOAT_ITEM_H

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
    Q_PLUGIN_METADATA(IID "org.kde.marble.NavigationFloatItem")

    Q_INTERFACES( Marble::RenderPluginInterface )

MARBLE_PLUGIN( NavigationFloatItem )

 public:
    explicit NavigationFloatItem( const MarbleModel *marbleModel = 0 );
    ~NavigationFloatItem() override;

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

    static QPixmap pixmap( const QString &Id );

    QHash<QString,QVariant> settings() const override;

    void setSettings( const QHash<QString, QVariant> &settings ) override;

 protected:
    bool eventFilter( QObject *object, QEvent *e ) override;
    void paintContent( QPainter *painter ) override;
    void contextMenuEvent( QWidget *w, QContextMenuEvent *e ) override;

 private Q_SLOTS:
    /** Map theme was changed, adjust controls */
    void selectTheme( const QString& theme );

    /** Enable/disable zoom in/out buttons */
    void updateButtons( int zoomValue );

    void activateCurrentPositionButton();
    void activateHomeButton();
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

    bool m_showHomeButton;
};

}

#endif // NAVIGATION_FLOAT_ITEM_H

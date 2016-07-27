//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef FILEVIEW_FLOAT_ITEM_H
#define FILEVIEW_FLOAT_ITEM_H

// forward declarations
class QListView;
class QPersistentModelIndex;

#include "AbstractFloatItem.h"

namespace Marble
{

class MarbleWidget;

/**
 * @short Provides a float item with a list of opened files
 *
 */
class FileViewFloatItem: public AbstractFloatItem
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.FileViewFloatItem")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN(FileViewFloatItem)

 public:
    explicit FileViewFloatItem( const QPointF &point = QPointF( -1, 10 ),
                            const QSizeF &size = QSizeF( 110.0, 250.0 ) );
    ~FileViewFloatItem();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;

    void initialize ();

    bool isInitialized () const;

    void changeViewport( ViewportParams *viewport );

    virtual QPainterPath backgroundShape() const;

    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer *layer = 0 );

 protected:
    bool eventFilter( QObject *object, QEvent *e );

 private Q_SLOTS:
    /** Map theme was changed, adjust controls */
    void selectTheme( const QString& theme );

    /** Enable/disable zoom in/out buttons */
    void updateFileView();

    void contextMenu(const QPoint& pos);

    void addFile();

    void removeFile();

 private:
    /** MarbleWidget this float item is installed as event filter for */
    MarbleWidget *m_marbleWidget;

    /** FileView controls */
    QListView *m_fileView;

    /** FileView embedding widget */
    QWidget *m_fileViewParent;

    /** current position */
    QPoint m_itemPosition;

    /** Radius of the viewport last time */
    int m_oldViewportRadius;

    /** Repaint needed */
    bool m_repaintScheduled;

    /** the last clicked ModelIndex */
    QPersistentModelIndex* m_persIndex;
};

}

#endif // FILEVIEW_FLOAT_ITEM_H

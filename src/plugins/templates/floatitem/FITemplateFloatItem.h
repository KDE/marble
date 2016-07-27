//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Inge Wallin <inge@lysator.liu.se>
//

//
// This class is a template Float Item plugin.
//


#ifndef FITEMPLATE_FLOAT_ITEM_H
#define FITEMPLATE_FLOAT_ITEM_H


// Qt

// Marble
#include "AbstractFloatItem.h"

class QSvgRenderer;

namespace Marble
{

/**
 * @short The class that creates a ... Float Item
 *
 */

class FITemplateFloatItem  : public AbstractFloatItem
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.FITemplateFloatItem")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN(FITemplateFloatItem)

 public:
    explicit FITemplateFloatItem( const QPointF &point = QPointF( -1.0, 10.0 ),
				  const QSizeF &size = QSizeF( 75.0, 75.0 ) );

    // ----------------------------------------------------------------
    // The following functions are defined in RenderPluginInterface.h
    // and MUST be part of the plugin.  See that file for documentation.
    //
    // Note that the class AbstractFloatItem provides default 
    // implementations for many of them.
    //

    ~FITemplateFloatItem ();

    QStringList backendTypes() const;

    // Provided by AbstractFloatItem and should not be implemented.
    //
    // QString renderPolicy() const;
    // QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon() const;

    void initialize();

    bool isInitialized() const;

    // Provided by AbstractFloatItem and should not be implemented.
    //
    // bool render( GeoPainter *painter, ViewportParams *viewport,
    //              const QString &renderPos, GeoSceneLayer *layer);

    QPainterPath backgroundShape() const;

    // End of RenderPluginInterface functions.
    // ----------------------------------------------------------------

    bool needsUpdate( ViewportParams *viewport );

    bool renderFloatItem( GeoPainter *painter, ViewportParams *viewport,
			  GeoSceneLayer * layer = 0 );

 private:
    Q_DISABLE_COPY( FITemplateFloatItem )

    QSvgRenderer  *m_svgobj;
    QPixmap        m_compass;

    /// allowed values: -1, 0, 1; default here: 0. FIXME: Declare enum
    int            m_polarity;
};

}

#endif

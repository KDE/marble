//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//

#ifndef SCENEGRAPHICSITEM_H
#define SCENEGRAPHICSITEM_H

#include <QRect>
#include <QObject>
#include <QPainterPath>
#include <QMouseEvent>

#include "GeoGraphicsItem.h"
#include "marble_export.h"


namespace Marble
{

class GeoDataPlacemark;

/**
 * @brief This is the base class for all scene graphics included within the
 * annotate plugin. It is not instantiated by itself but it is always used
 * as a part of a derived object.
 */
class SceneGraphicsItem : public GeoGraphicsItem
{
public:
    explicit SceneGraphicsItem( GeoDataPlacemark *placemark );
    ~SceneGraphicsItem();

    /**
     * @brief Returns the list of regions which form the scene graphic element.
     */
    QList<QRegion> regions() const;

    /**
     * @brief A setter for the m_regions private member.
     */
    void setRegions( const QList<QRegion> &regions );

    /**
     * @brief SceneGraphicItem class, when called from one of its derived classes'
     * constructors, takes as a parameter a pointer to the placemark of the graphic
     * element.
     * @return The pointer to the placemark mentioned above.
     */
    const GeoDataPlacemark *placemark() const;

    GeoDataPlacemark *placemark();

    /**
     * @brief This function is used to call the event distributer and makes use of
     * the re-implemented virtual functions which handle the mouse events.
     *
     * FIXME: There is still doubt whether there is a better way to do this or not.
     */
    bool sceneEvent( QEvent *event );

    /**
     * @brief It is used for downcasting a SceneGraphicItem. It returns a const char
     * which is the name of the element's class and is defined within the
     * SceneGraphicsTypes namespace.
     */
    virtual const char *graphicType() const = 0;

protected:
    /**
     * @brief Pure virtual functions which handle the mouse events, all of which are
     * re-implemented in every SceneGraphicItem derived classes.
     */
    virtual bool mousePressEvent( QMouseEvent *event ) = 0;
    virtual bool mouseMoveEvent( QMouseEvent *event ) = 0;
    virtual bool mouseReleaseEvent( QMouseEvent *event ) = 0;

private:
    QList<QRegion> m_regions;
    GeoDataPlacemark *m_placemark;
};

}

#endif // SCENEGRAPHICSITEM_H

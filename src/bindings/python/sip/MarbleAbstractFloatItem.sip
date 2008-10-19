//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

class MarbleAbstractFloatItem : Marble::MarbleRenderPlugin
{
%TypeHeaderCode
#include <marble/MarbleAbstractFloatItem.h>
%End

public:
    explicit                MarbleAbstractFloatItem (const QPointF& point = QPointF(10.0,10.0), const QSizeF& size = QSizeF(150.0,50.0));
    void                    setPosition (const QPointF& position);
    QPointF                 position () const;
    QPointF                 positivePosition (const QRectF& viewPort) const;
    void                    setSize (const QSizeF& size);
    virtual QSizeF          size () const;
    QPen                    pen () const;
    void                    setPen (const QPen& pen);
    QBrush                  background () const;
    void                    setBackground (const QBrush& background);
    QRectF                  contentRect () const;
    QRectF                  renderedRect () const;
    virtual QPainterPath    backgroundShape () const;
    void                    renderBackground (QPainter*);
    qreal                   border () const;
    void                    setBorder (qreal width);
    QBrush                  borderBrush () const;
    void                    setBorderBrush (const QBrush& brush);
    Qt::PenStyle            borderStyle () const;
    void                    setBorderStyle (Qt::PenStyle);
    QFont                   font () const;
    void                    setFont (const QFont& font);
    qreal                   margin () const;
    void                    setMargin (qreal margin);
    qreal                   marginTop () const;
    void                    setMarginTop (qreal marginTop);
    qreal                   marginBottom () const;
    void                    setMarginBottom (qreal marginBottom);
    qreal                   marginLeft () const;
    void                    setMarginLeft (qreal marginLeft);
    qreal                   marginRight () const;
    void                    setMarginRight (qreal marginRight);
    qreal                   padding () const;
    void                    setPadding (qreal width);
    bool                    positionLocked () const;
    void                    setPositionLocked (bool enabled);
    virtual bool            needsUpdate (Marble::ViewportParams* viewport);
    bool                    pixmapCacheEnabled () const;
    void                    setPixmapCacheEnabled (bool enabled);


//mt undefined symbol: GeoSceneLayer* layer -- need mapped type?
//mt     bool               render (Marble::GeoPainter* painter, Marble::ViewportParams* viewport, const QString& pos = "FLOAT_ITEM", GeoSceneLayer* layer = 0);




//mt undefined symbol: GeoSceneLayer* layer -- need mapped type?
//mt     virtual bool       renderFloatItem (Marble::GeoPainter* painter, Marble::ViewportParams* viewport, GeoSceneLayer* layer = 0);




//mt undefined symbol: GeoSceneLayer* layer -- need mapped type?
//mt     virtual bool       renderOnMap (Marble::GeoPainter* painter, Marble::ViewportParams* viewport, const QString& renderPos, GeoSceneLayer* layer = 0);


    virtual QString         renderPolicy () const;
    virtual QStringList     renderPosition () const;

protected:
    bool                    eventFilter (QObject*, QEvent*);

private:
                            MarbleAbstractFloatItem (const Marble::MarbleAbstractFloatItem&);
};   // MarbleAbstractFloatItem

};   // Marble



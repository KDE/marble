/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2008 Simon Schmeißer <mail_to_wrt@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "GeoShape.h"

#include <KoImageData.h>
#include <KoViewConverter.h>
#include <KoImageCollection.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoStoreDevice.h>
#include <KoUnit.h>

#include <QPainter>
#include <kdebug.h>

#include <marble/MarbleMap.h>
#include <marble/ClipPainter.h>


GeoShape::GeoShape()
{
    //setKeepAspectRatio(true);

   kDebug() << "map flake is created";

    m_marbleMap = new MarbleMap();
    m_marbleMap->setMapThemeId( "earth/bluemarble/bluemarble.dgml" );
    m_marbleMap->setProjection( Equirectangular );
    m_marbleMap->zoomView( 1050 );
}

GeoShape::~GeoShape()
{
    delete m_marbleMap;
}

KoShape * GeoShape::cloneShape() const
{
    // TODO test cloning
    MarbleMap *clone;
    clone = new MarbleMap( m_marbleMap->model() );
    return 0;//clone;
}

void GeoShape::paint( QPainter& painter, const KoViewConverter& converter ) {
    QRectF target = converter.documentToView(QRectF(QPointF(0,0), size()));
    QSize iSize = size().toSize();

    m_marbleMap->setSize( iSize.width(), iSize.height() );
    

    QPixmap temp( iSize );
    ClipPainter cp( &temp, false );
    QRect rect( QPoint(0,0), iSize );
    m_marbleMap->paint( cp, rect );

   painter.drawPixmap(target.toRect(), temp);

}

void GeoShape::saveOdf( KoShapeSavingContext & context ) const
{
   /* // make sure we have a valid image data pointer before saving
    KoImageData * data = m_imageData;
    if( data != userData() )
        data = dynamic_cast<KoImageData*> (userData());
    if(data == 0)
        return;
*/
    KoXmlWriter &writer = context.xmlWriter();

    const bool nestedInFrame = context.isSet(KoShapeSavingContext::FrameOpened);
    if( ! nestedInFrame ) {
        writer.startElement( "draw:frame" );
        saveOdfFrameAttributes(context);
    }
    saveOdfAttributes(context, 0); // required to clear the 'frameOpened' attribute on KoShape
    
    //writer.startElement("draw:object");

    writer.startElement("koffice:map");
    // In the spec, only the xlink:href attribute is marked as mandatory, cool :)
    //QString name = context.addImageForSaving( data->pixmap() );
    //writer.addAttribute("xlink:href", name);
    writer.addAttribute("mapThemeId", m_marbleMap->mapThemeId());
    writer.addAttribute("zoom", m_marbleMap->zoom());
    writer.addAttribute("centerLongitude", m_marbleMap->centerLongitude());
    writer.addAttribute("centerLatitude", m_marbleMap->centerLatitude());
    writer.endElement();

    //writer.endElement();

    if(! nestedInFrame)
        writer.endElement(); // draw-frame
}

bool GeoShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context )
{
    Q_UNUSED(context);

   const QString mapThemeId = element.attribute("mapThemeId");

    if ( !mapThemeId.isEmpty() )
    {
        m_marbleMap->setMapThemeId(mapThemeId);
    }

    const QString zoom = element.attribute("zoom");
    
    if ( !zoom.isEmpty() )
    {
        m_marbleMap->zoomView( zoom.toInt() );
    }
    else
        m_marbleMap->zoomView( 1050 );

    const QString centerLatitude = element.attribute("centerLatitude");

    if ( !centerLatitude.isEmpty() )
    {
        m_marbleMap->setCenterLatitude( centerLatitude.toDouble() );
    }

    const QString centerLongitude = element.attribute("centerLongitude");

    if ( !centerLongitude.isEmpty() )
    {
        m_marbleMap->setCenterLongitude( centerLongitude.toDouble() );
    }

    
    qDebug() << "map theme: loadOdf is getting called";
    qDebug() << zoom;

  /*  // the frame attributes are loaded outside in the shape registry
    if( m_imageCollection)//context.imageCollection() )
    {
        const QString href = element.attribute("href");

        KoImageData * data = new KoImageData( m_imageCollection);//context.imageCollection() );
        data->setStoreHref( href );
        setUserData( data );
    }
*/
    return true;
}

bool GeoShape::loadFromUrl( KUrl &url )
{
  /*  KoImageData * data = new KoImageData( m_imageCollection );
    QImage img(url.path());
    data->setImage(img);
    setUserData( data );
    setSize(QSizeF(DM_TO_POINT(img.width() / (double) img.dotsPerMeterX() * 10.0),
DM_TO_POINT(img.height() / (double) img.dotsPerMeterY() * 10.0)  ));
   */ return true;
}

void GeoShape::init(QMap<QString, KoDataCenter *>  dataCenterMap)
{
  //  m_imageCollection = (KoImageCollection *)dataCenterMap["ImageCollection"];
}

MarbleMap* GeoShape::marbleMap()
{
    return m_marbleMap;
}


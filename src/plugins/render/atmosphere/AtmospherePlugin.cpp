//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008-2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2008-2009 Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
// Copyright 2012      Kovalevskyy Illya <illya.kovalevskyy@gmail.com>
//

#include "AtmospherePlugin.h"
#include "Planet.h"

#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleModel.h"

#include <QIcon>

namespace Marble
{

AtmospherePlugin::AtmospherePlugin() :
    RenderPlugin( 0 ),
    m_renderRadius(-1)
{
}

AtmospherePlugin::AtmospherePlugin( const MarbleModel *marbleModel ) :
    RenderPlugin( marbleModel ),
    m_renderRadius(-1)
{
    connect( marbleModel, SIGNAL(themeChanged(QString)),
             this, SLOT(updateTheme()) );
}

QStringList AtmospherePlugin::backendTypes() const
{
    return QStringList(QStringLiteral("atmosphere"));
}

QString AtmospherePlugin::renderPolicy() const
{
    return QStringLiteral("SPECIFIED_ALWAYS");
}

QStringList AtmospherePlugin::renderPosition() const
{
    return QStringList(QStringLiteral("SURFACE"));
}

RenderPlugin::RenderType AtmospherePlugin::renderType() const
{
    return RenderPlugin::ThemeRenderType;
}

QString AtmospherePlugin::name() const
{
    return tr( "Atmosphere" );
}

QString AtmospherePlugin::guiString() const
{
    return tr( "&Atmosphere" );
}

QString AtmospherePlugin::nameId() const
{
    return QStringLiteral("atmosphere");
}

QString AtmospherePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString AtmospherePlugin::description() const
{
    return tr( "Shows the atmosphere around the earth." );
}

QIcon AtmospherePlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/atmosphere.png"));
}

QString AtmospherePlugin::copyrightYears() const
{
    return QStringLiteral("2006-2012");
}

QVector<PluginAuthor> AtmospherePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Torsten Rahn"), QStringLiteral("tackat@kde.org"))
            << PluginAuthor(QStringLiteral("Inge Wallin"), QStringLiteral("ingwa@kde.org"))
            << PluginAuthor(QStringLiteral("Jens-Michael Hoffmann"), QStringLiteral("jmho@c-xx.com"))
            << PluginAuthor(QStringLiteral("Patrick Spendrin"), QStringLiteral("ps_ml@gmx.de"))
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"))
            << PluginAuthor(QStringLiteral("Mohammed Nafees"), QStringLiteral("nafees.technocool@gmail.com"));
}

qreal AtmospherePlugin::zValue() const
{
    return -100.0;
}

void AtmospherePlugin::initialize()
{
    /* nothing to do */
}

bool AtmospherePlugin::isInitialized() const
{
    return true;
}

void AtmospherePlugin::updateTheme()
{
    bool hasAtmosphere = marbleModel()->planet()->hasAtmosphere();
    setEnabled( hasAtmosphere );
    setVisible( hasAtmosphere );
}

bool AtmospherePlugin::render( GeoPainter *painter,
                              ViewportParams *viewParams,
                              const QString &renderPos,
                              GeoSceneLayer *layer )
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    if ( !visible()  || !marbleModel()->planet()->hasAtmosphere() )
        return true;

    // Only draw an atmosphere if projection is spherical
    if ( viewParams->projection() != Spherical && viewParams->projection() != VerticalPerspective )
        return true;

    // No use to draw atmosphere if it's not visible in the area.
    if ( viewParams->mapCoversViewport() )
        return true;

    // Gradient should be recalculated only if planet color or size changed
    if(viewParams->radius() != m_renderRadius || marbleModel()->planet()->atmosphereColor() != m_renderColor) {
        m_renderRadius = viewParams->radius();
        m_renderColor = marbleModel()->planet()->atmosphereColor();
        repaintPixmap(viewParams);
    }
    int  imageHalfWidth  = viewParams->width() / 2;
    int  imageHalfHeight = viewParams->height() / 2;
    painter->drawPixmap(imageHalfWidth  - (int) ( (qreal) ( viewParams->radius() ) * 1.05 ),
                        imageHalfHeight - (int) ( (qreal) ( viewParams->radius() ) * 1.05 ),
                        m_renderPixmap);
    return true;
}

void AtmospherePlugin::repaintPixmap(const ViewportParams *viewParams)
{
    int  imageHalfWidth  = 1.05 * viewParams->radius();
    int  imageHalfHeight = 1.05 * viewParams->radius();

    int diameter = (int) ( 2.1 * (qreal) ( viewParams->radius()));
    m_renderPixmap = QPixmap(diameter, diameter);
    m_renderPixmap.fill(QColor(Qt::transparent));

    QPainter renderPainter(&m_renderPixmap);

    QColor color = marbleModel()->planet()->atmosphereColor();

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad( QPointF( imageHalfWidth, imageHalfHeight ),
                           1.05 * viewParams->radius() );
    grad.setColorAt( 0.91, color );
    grad.setColorAt( 1.00, QColor(color.red(), color.green(), color.blue(), 0) );

    QBrush brush(grad);
    renderPainter.setBrush(brush);
    renderPainter.setPen(Qt::NoPen);
    renderPainter.setRenderHint(QPainter::Antialiasing, false);

    // Let's paint elipse we want in this::render(..) on pixmap from point (0;0)
    renderPainter.drawEllipse(0, 0, diameter, diameter);
}

}

#include "moc_AtmospherePlugin.cpp"

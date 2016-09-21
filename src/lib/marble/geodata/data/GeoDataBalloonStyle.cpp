//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataBalloonStyle.h"
#include "GeoDataTypes.h"

#include <QDataStream>

namespace Marble
{

class GeoDataBalloonStylePrivate
{
public:
    GeoDataBalloonStylePrivate();

    QColor m_bgColor;
    QColor m_textColor;
    QString m_text;
    GeoDataBalloonStyle::DisplayMode m_mode;
};

GeoDataBalloonStylePrivate::GeoDataBalloonStylePrivate() :
    m_bgColor( Qt::white ),
    m_textColor( Qt::black ),
    m_mode( GeoDataBalloonStyle::Default )
{
}

GeoDataBalloonStyle::GeoDataBalloonStyle() :
    d( new GeoDataBalloonStylePrivate )
{
}

GeoDataBalloonStyle::GeoDataBalloonStyle( const Marble::GeoDataBalloonStyle &other ) :
    GeoDataColorStyle( other ), d( new GeoDataBalloonStylePrivate( *other.d ) )
{
}

GeoDataBalloonStyle &GeoDataBalloonStyle::operator=( const GeoDataBalloonStyle &other )
{
    GeoDataColorStyle::operator=(other);
    *d = *other.d;
    return *this;
}

bool GeoDataBalloonStyle::operator==( const GeoDataBalloonStyle &other ) const
{
    return equals(other) &&
           d->m_bgColor == other.d->m_bgColor &&
           d->m_mode == other.d->m_mode &&
           d->m_text == other.d->m_text &&
           d->m_textColor == other.d->m_textColor;
}

bool GeoDataBalloonStyle::operator!=( const GeoDataBalloonStyle &other ) const
{
    return !this->operator==(other);
}

GeoDataBalloonStyle::~GeoDataBalloonStyle()
{
    delete d;
}

const char *GeoDataBalloonStyle::nodeType() const
{
    return GeoDataTypes::GeoDataBalloonStyleType;
}

QColor GeoDataBalloonStyle::backgroundColor() const
{
    return d->m_bgColor;
}

void GeoDataBalloonStyle::setBackgroundColor( const QColor &color )
{
    d->m_bgColor = color;
}

QColor GeoDataBalloonStyle::textColor() const
{
    return d->m_textColor;
}

void GeoDataBalloonStyle::setTextColor( const QColor &color )
{
    d->m_textColor = color;
}

QString GeoDataBalloonStyle::text() const
{
    return d->m_text;
}

void GeoDataBalloonStyle::setText( const QString &text )
{
    d->m_text = text;
}

GeoDataBalloonStyle::DisplayMode GeoDataBalloonStyle::displayMode() const
{
    return d->m_mode;
}

void GeoDataBalloonStyle::setDisplayMode(DisplayMode mode)
{
    d->m_mode = mode;
}

void GeoDataBalloonStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << d->m_bgColor.name();
    stream << d->m_textColor.name();
    stream << d->m_text;
}

void GeoDataBalloonStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );

    stream >> d->m_bgColor;
    stream >> d->m_textColor;
    stream >> d->m_text;
}

}

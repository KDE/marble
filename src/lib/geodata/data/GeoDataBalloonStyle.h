//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATABALLOONSTYLE_H
#define GEODATABALLOONSTYLE_H

#include "MarbleGlobal.h"
#include "GeoDataColorStyle.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataBalloonStylePrivate;

class GEODATA_EXPORT GeoDataBalloonStyle : public GeoDataColorStyle
{
public:
    GeoDataBalloonStyle();

    GeoDataBalloonStyle( const GeoDataBalloonStyle &other );

    GeoDataBalloonStyle& operator=( const GeoDataBalloonStyle &other );

    ~GeoDataBalloonStyle();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    enum DisplayMode {
        Default,
        Hide
    };

    QColor backgroundColor() const;
    void setBackgroundColor( const QColor &color );

    QColor textColor() const;
    void setTextColor( const QColor &color );

    QString text() const;
    void setText( const QString &text );

    DisplayMode displayMode() const;
    void setDisplayMode( const DisplayMode &mode );

    virtual void pack( QDataStream& stream ) const;

    virtual void unpack( QDataStream& stream );

private:
    GeoDataBalloonStylePrivate* const d;
};

}

#endif

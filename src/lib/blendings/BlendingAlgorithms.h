// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_BLENDING_ALGORITHMS_H
#define MARBLE_BLENDING_ALGORITHMS_H

#include <QtCore/QtGlobal>

#include "Blending.h"

namespace Marble
{

class OverpaintBlending: public Blending
{
 public:
    virtual void blend( QImage * const bottom, TextureTile const * const top ) const;
};

class IndependentChannelBlending: public Blending
{
 public:
    virtual void blend( QImage * const bottom, TextureTile const * const top ) const;
 private:
    // bottomColorIntensity: intensity of one color channel (of one pixel) of the bottom image
    // topColorIntensity: intensity of one color channel (of one pixel) of the top image
    // return: intensity of the color channel (of a given pixel) of the result image
    // all color intensity values are in the range 0..1
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const = 0;
};


// Neutral blendings

class AllanonBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class ArcusTangentBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class GeometricMeanBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class LinearLightBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class NoiseBlending: public Blending // or IndependentChannelBlending?
{
};

class OverlayBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class ParallelBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class TextureBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};


// Darkening blendings

class ColorBurnBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class DarkBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class DarkenBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class DivideBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class GammaDarkBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class LinearBurnBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class MultiplyBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class SubtractiveBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};


// Lightening blendings

class AlphaBlending: public Blending
{
 public:
    virtual void blend( QImage * const bottom, TextureTile const * const top ) const;
};

class AdditiveBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class ColorDodgeBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class GammaLightBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class HardLightBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class LightBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class LightenBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class PinLightBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class ScreenBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class SoftLightBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class VividLightBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};


// Inverter blendings

class AdditiveSubtractiveBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class BleachBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class DifferenceBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class EquivalenceBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};

class HalfDifferenceBlending: public IndependentChannelBlending
{
    virtual qreal blendChannel( qreal const bottomColorIntensity,
                                qreal const topColorIntensity ) const;
};


// Special purpose blendings

class CloudsBlending: public Blending
{
 public:
    virtual void blend( QImage * const bottom, TextureTile const * const top ) const;
};

}

#endif

// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "BlendingFactory.h"

#include <QDebug>

#include "BlendingAlgorithms.h"
#include "blendings/SunLightBlending.h"

namespace Marble
{

void BlendingFactory::setLevelZeroLayout(int levelZeroColumns, int levelZeroRows)
{
    m_sunLightBlending->setLevelZeroLayout(levelZeroColumns, levelZeroRows);
}

Blending const *BlendingFactory::findBlending(QString const &name) const
{
    if (name.isEmpty())
        return nullptr;

    Blending const *const result = m_blendings.value(name, nullptr);
    if (!result)
        qWarning() << "Unknown blending" << name << "specified in .dgml file. Please fix the .dgml file.";
    return result;
}

BlendingFactory::BlendingFactory(const SunLocator *sunLocator)
    : m_sunLightBlending(new SunLightBlending(sunLocator))
{
    m_blendings.insert("OverpaintBlending", new OverpaintBlending);

    // Neutral blendings
    m_blendings.insert("AllanonBlending", new AllanonBlending);
    m_blendings.insert("ArcusTangentBlending", new ArcusTangentBlending);
    m_blendings.insert("GeometricMeanBlending", new GeometricMeanBlending);
    m_blendings.insert("LinearLightBlending", new LinearLightBlending);
    // m_blendings.insert( "NoiseBlending", new NoiseBlending );
    m_blendings.insert("OverlayBlending", new OverlayBlending);
    // m_blendings.insert( "ParallelBlending", new ParallelBlending );
    // m_blendings.insert( "TextureBlending", new TextureBlending );

    // Darkening blendings
    m_blendings.insert("AlphaBlending", new OverpaintBlending); // for backwards compatibility
    m_blendings.insert("ColorBurnBlending", new ColorBurnBlending);
    m_blendings.insert("DarkBlending", new DarkBlending);
    m_blendings.insert("DarkenBlending", new DarkenBlending);
    m_blendings.insert("DivideBlending", new DivideBlending);
    m_blendings.insert("GammaDarkBlending", new GammaDarkBlending);
    m_blendings.insert("LinearBurnBlending", new LinearBurnBlending);
    m_blendings.insert("MultiplyBlending", new MultiplyBlending);
    m_blendings.insert("SubtractiveBlending", new SubtractiveBlending);

    // Lightening blendings
    m_blendings.insert("AdditiveBlending", new AdditiveBlending);
    m_blendings.insert("ColorDodgeBlending", new ColorDodgeBlending);
    m_blendings.insert("GammaLightBlending", new GammaLightBlending);
    m_blendings.insert("HardLightBlending", new HardLightBlending);
    m_blendings.insert("LightBlending", new LightBlending);
    m_blendings.insert("LightenBlending", new LightenBlending);
    m_blendings.insert("PinLightBlending", new PinLightBlending);
    m_blendings.insert("ScreenBlending", new ScreenBlending);
    m_blendings.insert("SoftLightBlending", new SoftLightBlending);
    m_blendings.insert("VividLightBlending", new VividLightBlending);

    // Inverter blendings
    // m_blendings.insert( "AdditiveSubtractiveBlending", new AdditiveSubtractiveBlending );
    m_blendings.insert("BleachBlending", new BleachBlending);
    m_blendings.insert("DifferenceBlending", new DifferenceBlending);
    m_blendings.insert("EquivalenceBlending", new EquivalenceBlending);
    m_blendings.insert("HalfDifferenceBlending", new HalfDifferenceBlending);

    // Special purpose blendings
    m_blendings.insert("CloudsBlending", new CloudsBlending);
    m_blendings.insert("SunLightBlending", m_sunLightBlending);
    m_blendings.insert("GrayscaleBlending", new GrayscaleBlending);
    m_blendings.insert("InvertColorBlending", new InvertColorBlending);
    m_blendings.insert("InvertHueBlending", new InvertHueBlending);
}

BlendingFactory::~BlendingFactory()
{
    m_blendings.remove("SunLightBlending");
    delete m_sunLightBlending;
    qDeleteAll(m_blendings);
}

}

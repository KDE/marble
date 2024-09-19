// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_BLENDING_FACTORY_H
#define MARBLE_BLENDING_FACTORY_H

#include <QHash>

class QString;

namespace Marble
{
class Blending;
class SunLightBlending;
class SunLocator;

class BlendingFactory
{
public:
    explicit BlendingFactory(const SunLocator *sunLocator);
    ~BlendingFactory();

    void setLevelZeroLayout(int levelZeroColumns, int levelZeroRows);

    Blending const *findBlending(QString const &name) const;

private:
    Q_DISABLE_COPY(BlendingFactory)
    SunLightBlending *const m_sunLightBlending;
    QHash<QString, Blending const *> m_blendings;
};

}

#endif

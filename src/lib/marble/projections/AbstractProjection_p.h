// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_ABSTRACTPROJECTIONPRIVATE_H
#define MARBLE_ABSTRACTPROJECTIONPRIVATE_H

namespace Marble
{

class AbstractProjection;

class AbstractProjectionPrivate
{
public:
    explicit AbstractProjectionPrivate(AbstractProjection *parent);

    virtual ~AbstractProjectionPrivate() = default;

    int levelForResolution(qreal resolution) const;

    qreal m_maxLat;
    qreal m_minLat;
    mutable qreal m_previousResolution;
    mutable qreal m_level;

    AbstractProjection *const q_ptr;
    Q_DECLARE_PUBLIC(AbstractProjection)
};

} // namespace Marble

#endif

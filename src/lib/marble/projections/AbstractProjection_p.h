//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_ABSTRACTPROJECTIONPRIVATE_H
#define MARBLE_ABSTRACTPROJECTIONPRIVATE_H


namespace Marble
{

class AbstractProjection;

class AbstractProjectionPrivate
{
  public:
    explicit AbstractProjectionPrivate( AbstractProjection * parent );

    virtual ~AbstractProjectionPrivate() { };

    int levelForResolution(qreal resolution) const;

    qreal  m_maxLat;
    qreal  m_minLat;
    mutable qreal  m_previousResolution;
    mutable qreal  m_level;

    AbstractProjection * const q_ptr;
    Q_DECLARE_PUBLIC( AbstractProjection )
};

} // namespace Marble

#endif

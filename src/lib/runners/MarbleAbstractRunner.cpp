//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "MarbleAbstractRunner.h"
#include "MarbleMap.h"

#include <QtCore/QThread>
#include <QtCore/QString>

namespace Marble
{

MarbleAbstractRunner::MarbleAbstractRunner(QObject *parent)
    : QThread(parent),
      m_placemarkModel(0),
      m_map(0)
{
}

GeoDataFeature::GeoDataVisualCategory MarbleAbstractRunner::category() const
{
    return GeoDataFeature::Default;
}

void MarbleAbstractRunner::setMap(MarbleMap * map)
{
    m_map = map;
}

MarbleMap * MarbleAbstractRunner::map()
{
    return m_map;
}

void MarbleAbstractRunner::parse(const QString &input)
{
    m_input = input;
}

}

#include "MarbleAbstractRunner.moc"

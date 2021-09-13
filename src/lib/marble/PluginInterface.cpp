// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//
#include "PluginInterface.h"

namespace Marble
{

PluginInterface::~PluginInterface()
{
}

QString PluginInterface::aboutDataText() const
{
    return QString();
}

}

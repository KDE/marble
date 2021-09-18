// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Carlos Licea <carlos _licea@hotmail.com>
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "TextureMapperInterface.h"

using namespace Marble;

TextureMapperInterface::TextureMapperInterface() :
    m_repaintNeeded( true )
{
}


TextureMapperInterface::~TextureMapperInterface()
{
}

void TextureMapperInterface::setRepaintNeeded()
{
    m_repaintNeeded = true;
}

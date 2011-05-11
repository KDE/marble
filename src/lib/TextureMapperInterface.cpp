//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de
//

#include "TextureMapperInterface.h"

using namespace Marble;

TextureMapperInterface::TextureMapperInterface( QObject * const parent )
    : QObject( parent ),
      m_tileLevel( 0 )
{
}


TextureMapperInterface::~TextureMapperInterface()
{
}


void TextureMapperInterface::setTileLevel( int tileLevel )
{
    //    mDebug() << "Texture Level was set to: " << tileLevel;
    m_tileLevel = tileLevel;
}


#include "TextureMapperInterface.moc"

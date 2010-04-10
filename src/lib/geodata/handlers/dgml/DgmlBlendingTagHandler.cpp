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

#include "DgmlBlendingTagHandler.h"

#include "blendings/BlendingFactory.h"
#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTexture.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace dgml
{
static GeoTagHandlerRegistrar registrar( GeoTagHandler::QualifiedName( dgmlTag_Blending,
                                                                       dgmlTag_nameSpace20 ),
                                         new DgmlBlendingTagHandler );

GeoNode* DgmlBlendingTagHandler::parse( GeoParser& parser ) const
{
    // Check whether the tag is valid
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( dgmlTag_Blending ));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( !parentItem.represents( dgmlTag_Texture ))
        return 0;

    // Attribute name, default to ""
    const QString name = parser.attribute( dgmlAttr_name ).trimmed();
    mDebug() << "DgmlBlendingTagHandler::parse" << name;
    Blending const * const blending = BlendingFactory::instance()->findBlending( name );
    if ( !blending )
        parser.raiseWarning( QString( "Unknown (or no) blending algorithm: '%1'" ).arg( name ));
    parentItem.nodeAs<GeoSceneTexture>()->setBlending( blending );
    return 0;
}


}
}

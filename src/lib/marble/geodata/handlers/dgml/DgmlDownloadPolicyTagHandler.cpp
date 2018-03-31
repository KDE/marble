/*
Copyright 2009, 2010  Jens-Michael Hoffmann <jmho@c-xx.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DgmlDownloadPolicyTagHandler.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

#include "MarbleGlobal.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace dgml
{
static GeoTagHandlerRegistrar handler( GeoParser::QualifiedName( dgmlTag_DownloadPolicy,
                                                                     dgmlTag_nameSpace20 ),
                                       new DgmlDownloadPolicyTagHandler );

// Error handling:
// Here it is not possible to return an error code or throw an exception
// so in case of an error we just ignore the element.

GeoNode* DgmlDownloadPolicyTagHandler::parse( GeoParser& parser ) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_DownloadPolicy)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( !parentItem.represents( dgmlTag_Texture ) && !parentItem.represents( dgmlTag_Vectortile ) ) {
        qCritical( "Parse error: parent element is not 'texture' or 'vectortile'" );
        return nullptr;
    }

    // Attribute usage
    DownloadUsage usage;
    const QString usageStr = parser.attribute( dgmlAttr_usage ).trimmed();
    if (usageStr == QLatin1String("Browse"))
        usage = DownloadBrowse;
    else if (usageStr == QLatin1String("Bulk"))
        usage = DownloadBulk;
    else {
        qCritical( "Parse error: invalid attribute downloadPolicy/@usage" );
        return nullptr;
    }

    // Attribute maximumConnections
    const QString maximumConnectionsStr = parser.attribute( dgmlAttr_maximumConnections ).trimmed();
    bool ok;
    const int maximumConnections = maximumConnectionsStr.toInt( &ok );
    if ( !ok ) {
        qCritical( "Parse error: invalid attribute downloadPolicy/@maximumConnections" );
        return nullptr;
    }

    parentItem.nodeAs<GeoSceneTileDataset>()->addDownloadPolicy( usage, maximumConnections );
    return nullptr;
}

}
}

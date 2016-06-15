#include "BaseFilter.h"

#include "GeoDataDocument.h"
#include "GeoDataGeometry.h"
#include "GeoDataObject.h"
#include "GeoDataTypes.h"

BaseFilter::BaseFilter(GeoDataDocument* document, const char *type) :
    m_document(document)
{
    foreach (GeoDataFeature* feature, m_document->featureList()) {
        if(feature->nodeType() == type) {
            m_objects.append(feature);
        }
    }
}

BaseFilter::~BaseFilter()
{

}

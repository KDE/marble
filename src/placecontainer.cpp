#include "placecontainer.h"


PlaceContainer::PlaceContainer()
{
    clear();
}


PlaceContainer::PlaceContainer( const QString& name )
    : m_name(name)
{
    clear();
}

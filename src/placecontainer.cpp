#include "placecontainer.h"

PlaceContainer::PlaceContainer(){
	clear();
}

PlaceContainer::PlaceContainer( QString name ) : m_name(name)  {
	clear();
}

#include "placefolder.h"

PlaceFolder::PlaceFolder(){
	clear();
}

PlaceFolder::PlaceFolder( QString name ) : m_name(name)  {
	clear();
}

//
// C++ Implementation: gpplacestorage
//
// Description: 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QtCore/QDebug>

#include "placemarkstorage.h"

PlaceMarkStorage::PlaceMarkStorage(){
	m_name = "";
}

PlaceMarkStorage::PlaceMarkStorage(QString name){
	m_name = name;
}

//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>"
//


// Local
#include "AbstractProjectionHelper.h"

// Marble
//#include "ViewportParams.h"


AbstractProjectionHelper::AbstractProjectionHelper()
{
}

AbstractProjectionHelper::~AbstractProjectionHelper()
{
}


void AbstractProjectionHelper::paintBase( GeoPainter     *painter, 
					  ViewportParams *viewport,
					  QPen            &pen,
					  QBrush          &brush,
					  bool            antialiasing)
{
}



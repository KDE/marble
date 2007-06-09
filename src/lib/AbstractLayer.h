//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>"
// Copyright 2007      Inge Wallin   <ingwa@kde.org>"
// Copyright 2007      Thomas Zander <zander@kde.org>"
//

/**
 * This Class is to represent the base class for each of the levels in the rendering
 * of the Marble display. It will be the base class of PlacemarkPainter and 
 * FIXME: list the other two rendering layers here
 * */
class AbstractLayer{
    ///an empty constructor, so far
    AbstrctLayer();
    
    /**
     * Method to simplify the retreval of the screen pixel position from a longditude
     * and latitude
     * \param _long the longitude of the point we want to find
     * \param _lat the latitude of the point we want to find 
     * \param rotAxis the Quaternion that represents the angle and rotation of the globe
     * \param screenWidth the width in pixels of the current view
     * \param screenHeight the height in pixels of the current view
     * \param radius FIXME: add the roll of the radius in this comment
     * */
    getPixelPosFromGeoPoint(float _long, float _lat, Quaternion rotAxis, int screenWidth,
         screenHeight,  float radius);
}

//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>"
//

//
// MarbleDataFacade is an object that provides a read-only interface
// to internal Marble data.  This is usable in e.g. plugins.
//

#ifndef MARBLEDATAFACADE_H
#define MARBLEDATAFACADE_H


/** @file
 * This file contains the headers for MarbleDataFacade
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 */



#include "marble_export.h"

#include <QtCore/QDateTime>
#include <QtCore/QString>

#include "global.h"

class QAbstractItemModel;

namespace Marble
{

class MarbleModel;
class MarbleGeoDataModel;
class MarbleDataFacadePrivate;
class GeoDataDocument;
class FileViewModel;
class Planet;

/**
 * @short A read-only interface to internal Marble data.
 *
 * This class provides a read-only interface to some internal data in
 * Marble.  Its intended use is for plugins that should be able to
 * access the data in question, but not change it.

 * @see MarbleModel
 */

class MARBLE_EXPORT MarbleDataFacade
{
 public:
    /**
     * @brief  Construct a new MarbleDataFacade.
     * @param model The MarbleModel we get the data from
     */
    explicit MarbleDataFacade( MarbleModel *model );
    ~MarbleDataFacade();

    qreal planetRadius() const;

    /**
     * @return a pointer to the current planet
     */
    const Planet* planet() const;

    QDateTime dateTime() const;

    QString target() const;

    MarbleGeoDataModel* geoDataModel();
    
    QAbstractItemModel* renderModel();
    
    FileViewModel* fileViewModel() const;
 private:
    MarbleDataFacadePrivate  * const d;
};

}

#endif // MARBLEMODEL_H

/*
    SPDX-FileCopyrightText: 2005-2007 Torsten Rahn <tackat@kde.org>
    SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_TILELOADERHELPER_H
#define MARBLE_TILELOADERHELPER_H

#include "marble_export.h"

namespace Marble
{

namespace TileLoaderHelper
{
/**
 * @brief Get the maximum number of tile rows for a given tile level.
 * @param levelZeroRows  the number of rows in level zero
 * @param level  the tile level
 * @return       the maximum number of rows that a map level was tiled into.
 *               If the tile level number is invalid then "-1" gets
 *               returned so this case of wrong input data can get caught
 *               by the code which makes use of it.
 */
MARBLE_EXPORT int levelToRow(int levelZeroRows, int level);

/**
 * @brief Get the maximum number of tile columns for a given tile level.
 * @param levelZeroColumns  the number of columns in level zero
 * @param level  the tile level
 * @return       the maximum number of columns that a map level was tiled into.
 *               If the tile level number is invalid then "-1" gets
 *               returned so this case of wrong input data can get caught
 *               by the code which makes use of it.
 */
MARBLE_EXPORT int levelToColumn(int levelZeroColumns, int level);

/**
 * @brief Get the tile level for the given maximum number of tile columns.
 * @param levelZeroRows  the number of rows in level zero
 * @param row    the maximum number of rows that a map level was tiled into.
 * @return       the corresponding tile level.
 *               If the number of rows is invalid then "-1" gets
 *               returned so this case of wrong input data can get caught
 *               by the code which makes use of it.
 */
MARBLE_EXPORT int rowToLevel(int levelZeroRows, int row);

/**
 * @brief Get the tile level for the given maximum number of tile columns.
 * @param levelZeroColumns  the number of columns in level zero
 * @param column the maximum number of columns that a map level was tiled into.
 * @return       the corresponding tile level.
 *               If the number of columns is invalid then "-1" gets
 *               returned so this case of wrong input data can get caught
 *               by the code which makes use of it.
 */
MARBLE_EXPORT int columnToLevel(int levelZeroColumns, int column);
}

}

#endif // MARBLE_TILELOADERHELPER_H

/*
    SPDX-FileCopyrightText: 2005-2007 Torsten Rahn <tackat@kde.org>
    SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TileLoaderHelper.h"

#include <cmath>

#include "MarbleDebug.h"

#include "MarbleGlobal.h"

namespace Marble
{

int TileLoaderHelper::levelToRow(int levelZeroRows, int level)
{
    if (level < 0) {
        mDebug() << QStringLiteral("TileLoaderHelper::levelToRow(): Invalid level: %1").arg(level);
        return 0;
    }
    return levelZeroRows << level;
}

int TileLoaderHelper::levelToColumn(int levelZeroColumns, int level)
{
    if (level < 0) {
        mDebug() << QStringLiteral("TileLoaderHelper::levelToColumn(): Invalid level: %1").arg(level);
        return 0;
    }
    return levelZeroColumns << level;
}

int TileLoaderHelper::rowToLevel(int levelZeroRows, int row)
{
    if (row < levelZeroRows) {
        mDebug() << QStringLiteral("TileLoaderHelper::rowToLevel(): Invalid number of rows: %1").arg(row);
        return 0;
    }
    return (int)(std::log((qreal)(row / levelZeroRows)) / std::log((qreal)2.0));
}

int TileLoaderHelper::columnToLevel(int levelZeroColumns, int column)
{
    if (column < levelZeroColumns) {
        mDebug() << QStringLiteral("TileLoaderHelper::columnToLevel(): Invalid number of columns: %1").arg(column);
        return 0;
    }
    return (int)(std::log((qreal)(column / levelZeroColumns)) / std::log((qreal)2.0));
}

}

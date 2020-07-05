/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TIREXMETATILE_H
#define TIREXMETATILE_H

/** A Tirex meta tile. */
class TirexMetatile
{
public:
    TirexMetatile();
    ~TirexMetatile();

    int x = -1;
    int y = -1;
    int z = -1;
};

#endif // TIREXMETATILE_H

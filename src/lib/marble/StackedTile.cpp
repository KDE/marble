// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2023 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008-2010 Jens-Michael Hoffmann <jensmh@gmx.de>
// SPDX-FileCopyrightText: 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "StackedTile.h"

#include "MarbleDebug.h"
#include "TextureTile.h"

using namespace Marble;

static const uint **jumpTableFromQImage32(const QImage &img)
{
    if (img.depth() != 48 && img.depth() != 32)
        return nullptr;

    const int height = img.height();
    const int bpl = img.bytesPerLine() / 4;
    const uint *data = reinterpret_cast<const QRgb *>(img.bits());
    const uint **jumpTable = new const uint *[height];

    for (int y = 0; y < height; ++y) {
        jumpTable[y] = data;
        data += bpl;
    }

    return jumpTable;
}

static const uchar **jumpTableFromQImage8(const QImage &img)
{
    if (img.depth() != 8 && img.depth() != 1)
        return nullptr;

    const int height = img.height();
    const int bpl = img.bytesPerLine();
    const uchar *data = img.bits();
    const uchar **jumpTable = new const uchar *[height];

    for (int y = 0; y < height; ++y) {
        jumpTable[y] = data;
        data += bpl;
    }

    return jumpTable;
}

// return channelwise average of colors c1 and c2
static inline uint colorMix50(uint c1, uint c2)
{
    return (((c1 ^ c2) & 0xfefefefeUL) >> 1) + (c1 & c2);
}

static inline uint colorMix75(uint c1, uint c2)
{
    return colorMix50(c1, colorMix50(c1, c2)); // 75% c1
}

static inline uint colorMix25(uint c1, uint c2)
{
    return colorMix50(colorMix50(c1, c2), c2); // 25% c1
}

StackedTile::StackedTile(const TileId &id, const QImage &resultImage, QList<QSharedPointer<TextureTile>> const &tiles)
    : Tile(id)
    , m_resultImage(resultImage)
    , m_depth(resultImage.depth())
    , m_isGrayscale(resultImage.isGrayscale())
    , m_tiles(tiles)
    , jumpTable8(jumpTableFromQImage8(m_resultImage))
    , jumpTable32(jumpTableFromQImage32(m_resultImage))
    , m_byteCount(calcByteCount(resultImage, tiles))
    , m_isUsed(false)
{
    Q_ASSERT(!tiles.isEmpty());

    if (jumpTable32 == nullptr && jumpTable8 == nullptr) {
        qWarning() << "Color depth" << m_depth << " is not supported.";
    }
}

StackedTile::~StackedTile()
{
    delete[] jumpTable32;
    delete[] jumpTable8;
}

uint StackedTile::pixel(int x, int y) const
{
    if (m_depth == 32 && !m_isGrayscale)
        return (jumpTable32)[y][x];

    if (m_depth == 8) {
        if (m_isGrayscale)
            return (jumpTable8)[y][x];
        else
            return m_resultImage.color((jumpTable8)[y][x]);
    }

    if (m_depth == 1 && !m_isGrayscale)
        return m_resultImage.color((jumpTable8)[y][x / 8] >> 7);

    return m_resultImage.pixel(x, y);
}

#define CHEAPHIGH
#ifdef CHEAPHIGH

uint StackedTile::pixelF(qreal x, qreal y, const QRgb &topLeftValue) const
{
    // Bilinear interpolation to determine the color of a subpixel
    int iX = (int)(x);
    int iY = (int)(y);

    qreal fY = 8 * (y - iY);

    // Interpolation in y-direction
    if ((iY + 1) < m_resultImage.height()) {
        QRgb bottomLeftValue = pixel(iX, iY + 1);

        QRgb leftValue;
        if (fY < 1)
            leftValue = topLeftValue;
        else if (fY < 3)
            leftValue = colorMix75(topLeftValue, bottomLeftValue);
        else if (fY < 5)
            leftValue = colorMix50(topLeftValue, bottomLeftValue);
        else if (fY < 7)
            leftValue = colorMix25(topLeftValue, bottomLeftValue);
        else
            leftValue = bottomLeftValue;

        // Interpolation in x-direction
        if (iX + 1 < m_resultImage.width()) {
            qreal fX = 8 * (x - iX);

            QRgb topRightValue = pixel(iX + 1, iY);
            QRgb bottomRightValue = pixel(iX + 1, iY + 1);

            QRgb rightValue;
            if (fY < 1)
                rightValue = topRightValue;
            else if (fY < 3)
                rightValue = colorMix75(topRightValue, bottomRightValue);
            else if (fY < 5)
                rightValue = colorMix50(topRightValue, bottomRightValue);
            else if (fY < 7)
                rightValue = colorMix25(topRightValue, bottomRightValue);
            else
                rightValue = bottomRightValue;

            QRgb averageValue;

            if (fX < 1)
                averageValue = leftValue;
            else if (fX < 3)
                averageValue = colorMix75(leftValue, rightValue);
            else if (fX < 5)
                averageValue = colorMix50(leftValue, rightValue);
            else if (fX < 7)
                averageValue = colorMix25(leftValue, rightValue);
            else
                averageValue = rightValue;

            return averageValue;
        } else {
            return leftValue;
        }
    } else {
        // Interpolation in x-direction
        if (iX + 1 < m_resultImage.width()) {
            qreal fX = 8 * (x - iX);

            if (fX == 0)
                return topLeftValue;

            QRgb topRightValue = pixel(iX + 1, iY);

            QRgb topValue;
            if (fX < 1)
                topValue = topLeftValue;
            else if (fX < 3)
                topValue = colorMix75(topLeftValue, topRightValue);
            else if (fX < 5)
                topValue = colorMix50(topLeftValue, topRightValue);
            else if (fX < 7)
                topValue = colorMix25(topLeftValue, topRightValue);
            else
                topValue = topRightValue;

            return topValue;
        }
    }

    return topLeftValue;
}

#else

uint StackedTile::pixelF(qreal x, qreal y, const QRgb &topLeftValue) const
{
    // Bilinear interpolation to determine the color of a subpixel

    int iX = (int)(x);
    int iY = (int)(y);

    qreal fY = y - iY;

    // Interpolation in y-direction
    if ((iY + 1) < m_resultImage.height()) {
        QRgb bottomLeftValue = pixel(iX, iY + 1);
        // blending the color values of the top left and bottom left point
        qreal ml_red = (1.0 - fY) * qRed(topLeftValue) + fY * qRed(bottomLeftValue);
        qreal ml_green = (1.0 - fY) * qGreen(topLeftValue) + fY * qGreen(bottomLeftValue);
        qreal ml_blue = (1.0 - fY) * qBlue(topLeftValue) + fY * qBlue(bottomLeftValue);

        // Interpolation in x-direction
        if (iX + 1 < m_resultImage.width()) {
            qreal fX = x - iX;

            QRgb topRightValue = pixel(iX + 1, iY);
            QRgb bottomRightValue = pixel(iX + 1, iY + 1);

            // blending the color values of the top right and bottom right point
            qreal mr_red = (1.0 - fY) * qRed(topRightValue) + fY * qRed(bottomRightValue);
            qreal mr_green = (1.0 - fY) * qGreen(topRightValue) + fY * qGreen(bottomRightValue);
            qreal mr_blue = (1.0 - fY) * qBlue(topRightValue) + fY * qBlue(bottomRightValue);

            // blending the color values of the resulting middle left
            // and middle right points
            int mm_red = (int)((1.0 - fX) * ml_red + fX * mr_red);
            int mm_green = (int)((1.0 - fX) * ml_green + fX * mr_green);
            int mm_blue = (int)((1.0 - fX) * ml_blue + fX * mr_blue);

            return qRgb(mm_red, mm_green, mm_blue);
        } else {
            return qRgb(ml_red, ml_green, ml_blue);
        }
    } else {
        // Interpolation in x-direction
        if (iX + 1 < m_resultImage.width()) {
            qreal fX = x - iX;

            if (fX == 0.0)
                return topLeftValue;

            QRgb topRightValue = pixel(iX + 1, iY);
            // blending the color values of the top left and top right point
            int tm_red = (int)((1.0 - fX) * qRed(topLeftValue) + fX * qRed(topRightValue));
            int tm_green = (int)((1.0 - fX) * qGreen(topLeftValue) + fX * qGreen(topRightValue));
            int tm_blue = (int)((1.0 - fX) * qBlue(topLeftValue) + fX * qBlue(topRightValue));

            return qRgb(tm_red, tm_green, tm_blue);
        }
    }

    return topLeftValue;
}

#endif

int StackedTile::calcByteCount(const QImage &resultImage, const QList<QSharedPointer<TextureTile>> &tiles)
{
    int byteCount = resultImage.sizeInBytes();

    QList<QSharedPointer<TextureTile>>::const_iterator pos = tiles.constBegin();
    QList<QSharedPointer<TextureTile>>::const_iterator const end = tiles.constEnd();
    for (; pos != end; ++pos)
        byteCount += (*pos)->byteCount();

    return byteCount;
}

void StackedTile::setUsed(bool used)
{
    m_isUsed = used;
}

bool StackedTile::used() const
{
    return m_isUsed;
}

uint StackedTile::pixelF(qreal x, qreal y) const
{
    int iX = (int)(x);
    int iY = (int)(y);

    QRgb topLeftValue = pixel(iX, iY);

    return pixelF(x, y, topLeftValue);
}

int StackedTile::depth() const
{
    return m_depth;
}

int StackedTile::byteCount() const
{
    return m_byteCount;
}

QList<QSharedPointer<TextureTile>> StackedTile::tiles() const
{
    return m_tiles;
}

QImage const *StackedTile::resultImage() const
{
    return &m_resultImage;
}

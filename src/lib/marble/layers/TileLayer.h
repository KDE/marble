#ifndef MARBLE_TILELAYER_H
#define MARBLE_TILELAYER_H

#include "GeoSceneAbstractTileProjection.h"
#include <LayerInterface.h>
#include <QObject>

namespace Marble
{

class MARBLE_EXPORT TileLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    TileLayer();

    QStringList renderPosition() const override;

    virtual QSize tileSize() const = 0;
    virtual const GeoSceneAbstractTileProjection *tileProjection() const = 0;

    virtual int tileColumnCount(int level) const = 0;
    virtual int tileRowCount(int level) const = 0;

    virtual int layerCount() const = 0;
};

}

#endif // MARBLE_TILELAYER_H

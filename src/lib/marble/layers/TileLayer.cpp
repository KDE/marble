#include "TileLayer.h"

namespace Marble
{

TileLayer::TileLayer()
{

}

QStringList TileLayer::renderPosition() const
{
    return QStringList(QStringLiteral("SURFACE"));
}


}

#include "moc_TileLayer.cpp"

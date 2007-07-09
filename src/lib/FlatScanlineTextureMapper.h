#ifndef __MARBLE__FLATSCANLINETEXTUREMAPPER_H
#define __MARBLE__FLATSCANLINETEXTUREMAPPER_H


#include <QtCore/QString>

#include "AbstractScanlineTextureMapper.h"


class FlatScanlineTextureMapper : public AbstractScanlineTextureMapper {
 public:
    FlatScanlineTextureMapper(const QString& path, QObject * parent = 0);
    void mapTexture(QImage* canvasImage, int, const Quaternion& planetAxis);
    
 private:
    float  m_oldCenterLng;
    int    m_oldYPaintedTop;
};
#endif

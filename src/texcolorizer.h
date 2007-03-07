//
// C++ Interface: texcolorizer
//
// Description: TextureColorizer 

// The TextureColorizer maps the Elevationvalues to Legend Colors.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution

#ifndef TEXCOLORIZER_H
#define TEXCOLORIZER_H

#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtCore/QList>
#include <QtCore/QVector>

/**
@author Torsten Rahn
*/

typedef struct {
	uchar x1, x2, x3, x4;	
} GpUint;

typedef union {
	uint buffer;
	GpUint gpuint;
} GpFifo;

class TextureColorizer {
public:
	TextureColorizer(QString, QString);
	virtual ~TextureColorizer(){}
	void colorize(QImage*, const QImage*, const int&);
private:
	static const uint texturepalette[][512];
};

#endif

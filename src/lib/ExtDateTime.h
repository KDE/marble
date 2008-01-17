// (c) 2007-2008 David Roberts

#ifndef EXTDATETIME_H
#define EXTDATETIME_H

#include <QDateTime>

class ExtDateTime : public QDateTime {
	public:
		explicit ExtDateTime();
		virtual ~ExtDateTime();
		void update();
		int year0();
		long toJDN();
		double dayFraction();
};

#endif

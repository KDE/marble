// (c) 2007-2008 David Roberts

#include "ExtDateTime.h"

ExtDateTime::ExtDateTime() {
	update();
}

ExtDateTime::~ExtDateTime() {
}

void ExtDateTime::update() {
	QDateTime datetime = currentDateTime().toUTC();
	setDate(datetime.date());
	setTime(datetime.time());
}

int ExtDateTime::year0() {
	int year = date().year();
	if(year < 0) year++; // convert 1BCE to year 0, etc
	return year;
}

long ExtDateTime::toJDN() {
	// convert to julian day number
	// adapted from http://en.wikipedia.org/wiki/Julian_day#Calculation
	const int EPOCH_G = 32045; // 29 February 4801BCE in gregorian calendar
	const int EPOCH_J = 32083; // 29 February 4801BCE in julian calendar
	
	int y = year0() + 4800;
	int m = date().month() - 3;
	
	if(date().month() <= 2) {
		y--;
		m += 12;
	}
	
	long jdn = date().day() + ((153*m + 2) / 5) + 365*y + y/4;
	
	if(jdn >= 2331254) {
		// if the date is >= 1582-10-15, then assume gregorian calendar is being used
		jdn += -y/100 + y/400 - EPOCH_G;
	} else {
		// assume julian calendar is being used
		jdn -= EPOCH_J;
	}
	
	return jdn;
}

double ExtDateTime::dayFraction() {
	double f;
	f = time().second();
	f = f/60.0 + time().minute();
	f = f/60.0 + time().hour();
	f = f/24.0;
	
	return f;
}

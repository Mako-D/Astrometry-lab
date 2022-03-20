#pragma once

#include "Coordinate.h"
#include "Tycho2access.h"

using Magnitude = double;
using DE = double;
using RA = double;
using CoordinatesEpoch = pair<RA, DE>;
using ProperMotion = pair<RA, DE>; // degree per year

enum class SpectralClass {
	O,
	B,
	A,
	F,
	G,
	K,
	M
};

class Star {
protected:
	Coordinates coord;
	ProperMotion pmotion;
	CoordinatesEpoch epoch_coord;
	Magnitude mag;
public:
	friend bool operator == (const Star& lhs, const Star& rhs) {
		return (
			lhs.coord == rhs.coord &&
			abs(lhs.mag - rhs.mag) < MagnitudeAccuracy
			);
	}
	RA Get_RA() const {
		return coord.Get_alpha();
	}
	DE Get_DE() const {
		return coord.Get_delta();
	}
	Magnitude Get_mag() const {
		return mag;
	}

};

class UCAC2STAR : public Star {
public:
	UCAC2STAR
	(Coordinates coord,
		ProperMotion pmotion,
		CoordinatesEpoch epoch_coord,
		Magnitude mag)
	{
		this->coord = coord;
		this->pmotion = pmotion;
		this->epoch_coord = epoch_coord;
		this->mag = mag;
	}
	UCAC2STAR()
	{
		this->coord = { 0., 0. };
		this->pmotion = { 0., 0. };
		this->epoch_coord = { 2000., 2000. };
		this->mag = 0.;
	}
	//void ConvertCharBufferToStars(const )
private:
	//int UCAC2ID;
	//UCAC2_STAR s;

	friend istream& operator >> (istream& is, UCAC2STAR& in) {
		string temp;
		string needed1, needed2;
		is >> temp >> needed1 >> needed2;
		in.coord = { atof(needed1.c_str()), atof(needed2.c_str()) };
		is >> needed1;
		in.mag = atof(needed1.c_str());
		is >> needed1 >> needed2;
		in.epoch_coord = make_pair(atof(needed1.c_str()), atof(needed2.c_str()));
		is >> temp >> temp >> temp >> temp >> temp >> temp;
		is >> needed1 >> needed2;
		in.pmotion = make_pair(atof(needed1.c_str()) * AngleMAS, atof(needed2.c_str()) * AngleMAS);
		is.ignore(200, '\n');

		return is;
	}
};

class TYCHO2STAR : public Star {
public:
	TYCHO2STAR
	(Coordinates coord,
		ProperMotion pmotion,
		Magnitude mag)
	{
		this->coord = coord;
		this->pmotion = pmotion;
		this->mag = mag;
	}
	TYCHO2STAR()
	{
		this->coord = { 0., 0. };
		this->pmotion = { 0., 0. };
		this->mag = 0.;
	}
	friend istream& operator >>(istream& is, TYCHO2STAR& ts) {
		string istring;
		getline(is, istring, '\n');
		ts = {
			{ GetValueTycho2_d(istring, Tycho2Label::mRAdeg),
			  GetValueTycho2_d(istring, Tycho2Label::mDEdeg) },
			{ GetValueTycho2_d(istring, Tycho2Label::pmRA),
			  GetValueTycho2_d(istring, Tycho2Label::pmDE) },
			  GetValueTycho2_d(istring, Tycho2Label::VT)
		};
		ts.sp = ts.Set_SpectralClass(GetValueTycho2_d(istring, Tycho2Label::BT),
			GetValueTycho2_d(istring, Tycho2Label::VT));
		return is;
	}
	SpectralClass Get_sp() const {
		return sp;
	}
private:
	SpectralClass sp;
	SpectralClass Set_SpectralClass(double B, double V) {
		// https://ads.harvard.edu/cgi-bin/bbrowse?book=hsaa&page=65
		SpectralClass sp;
		if (B - V > 1.40) {
			sp = SpectralClass::M;
		}
		else if (B - V > 0.81) {
			sp = SpectralClass::K;
		}
		else if (B - V > 0.58) {
			sp = SpectralClass::G;
		}
		else if (B - V > 0.30) {
			sp = SpectralClass::F;
		}
		else if (B - V > -0.02) {
			sp = SpectralClass::A;
		}
		else if (B - V > -0.30) {
			sp = SpectralClass::B;
		}
		else {
			sp = SpectralClass::O;
		}
		return sp;
	}
};
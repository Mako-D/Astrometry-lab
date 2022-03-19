#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <string>
#include "Second_Trash.h"
#include "Third_Trash.h"
#include <fstream>
#include <sstream>
#include <exception>
extern "C" {
#include "ucac2.h"
}

#define AreaAccuraccyDeg 1. / (60. * 60.)

using Magnitude = double;
using DE = double;
using RA = double;
using ProperMotion = pair<RA, DE>; // degree per year

class StarCatalog {
protected:
	Coordinates coord;
	ProperMotion pmotion;
	Magnitude mag;
public:
	friend bool operator == (const StarCatalog& lhs, const StarCatalog& rhs) {
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

};

class UCAC2STAR : public StarCatalog {
public:
	UCAC2STAR
	(Coordinates coord,
		ProperMotion pmotion,
		Magnitude mag)
	{
		this->coord = coord;
		this->pmotion = pmotion;
		this->mag = mag;
	}
	UCAC2STAR()
	{
		this->coord = { 0., 0. };
		this->pmotion = { 0., 0. };
		this->mag = 0.;
	}
	//void ConvertCharBufferToStars(const )
private:
	int offset;
	UCAC2_STAR s;
};

class TYCHO2STAR : public StarCatalog {
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
		return is;
	}
};

using Tycho2Catalog = vector<TYCHO2STAR>;
using UCAC2Catalog = vector<UCAC2STAR>;

int main() {
	Tycho2Catalog t2c;
	{
		ifstream t2c_stream("catalog.dat");
		if (t2c_stream.fail()) exit(1);
		TYCHO2STAR temp;
		while (!t2c_stream.eof() && t2c.size() < 1'000) {
			t2c_stream >> temp;
			t2c.push_back(temp);
		}
		t2c_stream.close();
	}

	for (const auto& t2s : t2c) {
		UCAC2Catalog u2c;
		{
			char buffer[1000] = { '\0' };
			char path[24] = "UCAC2Catalog\\u2\\extract";
			int a = extract_ucac2_stars(buffer, t2s.Get_RA(), t2s.Get_DE(), 
				AreaAccuraccyDeg, AreaAccuraccyDeg, path, 0);
			stringstream ss;
			string starlineUCAC2;
			ss << buffer;
			while (getline(ss, starlineUCAC2)) {
				cout << "HI";
			}
		}
	}


	return 0;
}
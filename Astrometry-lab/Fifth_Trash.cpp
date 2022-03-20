#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <string>
#include "Second_Trash.h"
#include "Third_Trash.h"
#include <fstream>
#include <sstream>
#include <map>
#include <omp.h>
extern "C" {
#include "ucac2.h"
}

#define AreaAccuraccyDeg 3. * 1. / (60. * 60.)

using Magnitude = double;
using DE = double;
using RA = double;
using CoordinatesEpoch = pair<RA, DE>;
using ProperMotion = pair<RA, DE>; // degree per year

enum SpectralClass {
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
	Magnitude Get_mag() const{
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
		return is;
	}
};

template<typename T, typename K>
pair<T, K> operator +(const pair<T, K>& lhs, const pair<T, K>& rhs) {
	return { lhs.first + rhs.first, lhs.second + rhs.second };
}

template<typename T, typename K>
pair<T, K> operator /(const pair<T, K>& lhs, const pair<T, K>& rhs) {
	return { lhs.first / rhs.first, lhs.second / rhs.second };
}

template<typename T, typename K>
pair<T, K> operator -(const pair<T, K>& lhs, const pair<T, K>& rhs) {
	return { lhs.first - rhs.first, lhs.second - rhs.second };
}

template<typename T, typename K>
pair<T, K> operator /(const pair<T, K>& lhs, double rhs) {
	return { lhs.first / rhs, lhs.second / rhs};
}

using Tycho2Catalog = vector<TYCHO2STAR>;
using IndentityCatalog = vector<pair<TYCHO2STAR, UCAC2STAR>>;


double CalcUncertaintyOfCatalogs(const IndentityCatalog& cat) {
	// #1.1 (1) stage
	vector<pair<double, double>> coord_uncert_1;
	{
		double Delta_RA = 0.;
		double Delta_DE = 0.;
		for (const auto& [t2s, u2s] : cat) {
			Delta_RA += u2s.Get_RA() - t2s.Get_RA();
			Delta_DE += u2s.Get_DE() - t2s.Get_DE();
		}
		Delta_RA /= static_cast<int>(cat.size());
		Delta_DE /= static_cast<int>(cat.size());

		// #1.2 (2) stage
		for (const auto& [t2s, u2s] : cat) {
			coord_uncert_1.push_back({ u2s.Get_RA() - Delta_RA, u2s.Get_DE() - Delta_DE });
		}
	}

	vector<pair<double, double>> coord_uncert_2;
	{
		// #2.1 (3) stage
	// Разбиваю звёзды по зонам, разбиение по RA
		map<int, vector<pair<double, double>>> zone_delim_RA_2;
		for (const auto& [u2RAe, u2DEe] : coord_uncert_1) {
			zone_delim_RA_2[int(u2RAe)].push_back({ u2RAe, u2DEe }); // from 0 to 359 zones (totally 360)
		}

		//Суммирую по Alpha_1_i
		map<int, pair<double, double>> Delta_coord_RA;
		for (const auto& [zone, star] : zone_delim_RA_2) {
			for (const auto& coord : star) {
				Delta_coord_RA[zone].first += coord.first;
				Delta_coord_RA[zone].second += coord.second;
			}
		}
		//Делю на число звёзд в зоне
		for (auto& [zone, coord] : Delta_coord_RA) {
			coord.first /= static_cast<int>(zone_delim_RA_2[zone].size());
			coord.second /= static_cast<int>(zone_delim_RA_2[zone].size());
		}

		// 2.2 (4) stage
		for (const auto& a_1 : coord_uncert_1) {
			coord_uncert_2.push_back({
				a_1.first - Delta_coord_RA[int(a_1.first)].first,
				a_1.second - Delta_coord_RA[int(a_1.first)].second
				});
		}
	}
	
	vector<pair<double, double>> coord_uncert_3;
	{
		// #3.1 (5) stage
		map<int, vector<pair<double, double>>> zone_delim_DE_2;
		for (const auto& [u2RAe, u2DEe] : coord_uncert_2) {
			zone_delim_DE_2[int(u2DEe)].push_back({ u2RAe, u2DEe }); // from -89 to 89 zones (totally 179)
		}

		map<int, pair<double, double>> Delta_coord_DE;
		for (const auto& [zone, star] : zone_delim_DE_2) {
			for (const auto& coord : star) {
				Delta_coord_DE[zone] = Delta_coord_DE[zone] + coord;
			}
		}
		for (auto& [zone, coord] : Delta_coord_DE) {
			coord.first /= static_cast<int>(zone_delim_DE_2[zone].size());
			coord.second /= static_cast<int>(zone_delim_DE_2[zone].size());
		}

		// 3.2 (6) stage
		for (const auto& a_2 : coord_uncert_2) {
			coord_uncert_3.push_back({
				a_2.first - Delta_coord_DE[int(a_2.second)].first,
				a_2.second - Delta_coord_DE[int(a_2.second)].second
				});
		}
	}

	vector<pair<double, double>> coord_uncert_4;
	{
		// 4.1 (7) stage
		//extract magnitude list
		vector<Magnitude> mag_u2_list;
		for (const auto& [t2s, u2s] : cat) {
			mag_u2_list.push_back(u2s.Get_mag());
		}
		
		map<int, vector<pair<double, double>>> zone_delim_mag;
		for (size_t i = 0; i < coord_uncert_3.size(); i++) {
			zone_delim_mag[int(mag_u2_list[i])].push_back(coord_uncert_3[i]);
		}

		map<int, pair<double, double>> Delta_coord_mag;
		for (const auto& [zone, coord] : zone_delim_mag) {
			Delta_coord_mag[zone] = Delta_coord_mag[zone] + coord[zone];
		}
		for (auto& [zone, coord] : Delta_coord_mag) {
			coord = coord / int(zone_delim_mag[zone].size());
		}

		// 4.2 (8) stage
		for (size_t i = 0; i < coord_uncert_3.size(); i++) {
			coord_uncert_4.push_back(
				coord_uncert_3[i] - Delta_coord_mag[mag_u2_list[i]]
				);
		}
	}

	return 1.;
}

int main() {
	IndentityCatalog ICatalog;
	{
		Tycho2Catalog t2c;
		{
			ifstream t2c_stream("catalog.dat");
			if (t2c_stream.fail()) exit(1);
			TYCHO2STAR temp;
			while (!t2c_stream.eof() && t2c.size() < 300'000) {
				t2c_stream >> temp;
				t2c.push_back(temp);
			}
			t2c_stream.close();
		}

		//auto start_time = std::chrono::steady_clock::now();
		{
			for (const auto& t2s : t2c) {
				char buffer[4000] = { '\0' };
				char path[24] = "UCAC2Catalog\\u2\\extract";

				int a = extract_ucac2_stars(buffer, t2s.Get_RA(), t2s.Get_DE(),
					AreaAccuraccyDeg, AreaAccuraccyDeg, path, 0);

				stringstream ss;
				string starlineUCAC2;
				ss << buffer;

				while (getline(ss, starlineUCAC2)) {
					stringstream ss_additional;
					UCAC2STAR u2s;
					ss_additional << starlineUCAC2;
					ss_additional >> u2s;
					if (t2s == u2s) {
						ICatalog.push_back({ t2s, u2s });
						break;
					}
				}
			}
		}
		//auto end_time = std::chrono::steady_clock::now();
		//auto elapsed_ns = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		//std::cout << elapsed_ns.count() << " ms\n";
	}
	double fuck = CalcUncertaintyOfCatalogs(ICatalog);

	return 0;
}
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
using IdentityCatalog = vector<pair<TYCHO2STAR, UCAC2STAR>>;


double CalcUncertaintyOfCatalogs(const IdentityCatalog& cat) {
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
		for (const auto& [zone, star] : zone_delim_mag) {
			for (const auto& coord : star) {
				Delta_coord_mag[zone] = Delta_coord_mag[zone] + coord;
			}
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

	vector<pair<double, double>> coord_uncert_5;
	{
		// 5.1 (9) stage
		//extract spectral classes list
		vector<SpectralClass> sp_u2_list;
		for (const auto& [t2s, u2s] : cat) {
			sp_u2_list.push_back(t2s.Get_sp());
		}

		map<SpectralClass, vector<pair<double, double>>> zone_delim_sp;
		for (size_t i = 0; i < coord_uncert_4.size(); i++) {
			zone_delim_sp[sp_u2_list[i]].push_back(coord_uncert_4[i]);
		}

		map<SpectralClass, pair<double, double>> Delta_coord_sp;
		for (const auto& [zone, star] : zone_delim_sp) {
			for (const auto& coord : star) {
				Delta_coord_sp[zone] = Delta_coord_sp[zone] + coord;
			}
		}
		for (auto& [zone, coord] : Delta_coord_sp) {
			coord = coord / int(zone_delim_sp[zone].size());
		}

		//5.2 (10) stage
		for (size_t i = 0; i < coord_uncert_4.size(); i++) {
			coord_uncert_5.push_back(
				coord_uncert_4[i] - Delta_coord_sp[sp_u2_list[i]]
			);
		}
	}

	//6.1 (11) stage
	double RA_res = 0., DE_res = 0.;
	for (const auto& [RA, DE] : coord_uncert_5) {
		RA_res += RA;
		DE_res += DE;
	}

	RA_res /= static_cast<int>(coord_uncert_5.size());
	DE_res /= static_cast<int>(coord_uncert_5.size());

	return sqrt(RA_res * RA_res + DE_res * DE_res);
}

int main() {
	IdentityCatalog ICatalog;
	{
		Tycho2Catalog t2c;
		{
			ifstream t2c_stream("catalog.dat");
			if (t2c_stream.fail()) exit(1);
			TYCHO2STAR temp;
			while (!t2c_stream.eof() /*&& t2c.size() < 100'000*/) {
				t2c_stream >> temp;
				t2c.push_back(temp);
			}
			t2c_stream.close();
		}

		auto start_time1 = std::chrono::steady_clock::now();
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
		auto end_time1 = std::chrono::steady_clock::now();
		auto elapsed_ns1 = std::chrono::duration_cast<std::chrono::milliseconds>(end_time1 - start_time1);
		std::cout << "Indentification time is: " << elapsed_ns1.count() << " ms\n";
	}

	auto start_time = std::chrono::steady_clock::now();

	cout << "Total value of identical stars: " << ICatalog.size() << endl;
	cout << "Gamma value is: " << CalcUncertaintyOfCatalogs(ICatalog) << endl;

	auto end_time = std::chrono::steady_clock::now();
	auto elapsed_ns = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "Time of calc:" << elapsed_ns.count() << " ms\n";
	return 0;
}
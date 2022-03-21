#pragma once

#include "CatalogClasses.h"
#include <map>

template<typename T, typename K>
std::pair<T, K> operator +=(const std::pair<T, K>& lhs, const std::pair<T, K>& rhs) {
	return { lhs.first + rhs.first, lhs.second + rhs.second };
}

template<typename T, typename K>
std::pair<T, K> operator /=(const std::pair<T, K>& lhs, const std::pair<T, K>& rhs) {
	return { lhs.first / rhs.first, lhs.second / rhs.second };
}

template<typename T, typename K>
std::pair<T, K> operator -(const std::pair<T, K>& lhs, const std::pair<T, K>& rhs) {
	return { lhs.first - rhs.first, lhs.second - rhs.second };
}

template<typename T, typename K>
std::pair<T, K> operator /=(const std::pair<T, K>& lhs, double rhs) {
	return { lhs.first / rhs, lhs.second / rhs };
}

using Tycho2Catalog = std::vector<TYCHO2STAR>;
using IdentityCatalog = std::vector<std::pair<TYCHO2STAR, UCAC2STAR>>;


double CalcUncertaintyOfCatalogs(const IdentityCatalog& cat) {
	using namespace std;
	// #1.1 (1) stage
	vector<pair<double, double>> coord_uncert_1;
	{
		pair<double, double> Delta_coord = { 0., 0. };
		double Delta_RA = 0.;
		double Delta_DE = 0.;
		for (const auto& [t2s, u2s] : cat) {
			Delta_coord += make_pair(u2s.Get_RA() - t2s.Get_RA(), u2s.Get_DE() - t2s.Get_DE());
		}
		Delta_coord /= static_cast<int>(cat.size());

		// #1.2 (2) stage
		for (const auto& [t2s, u2s] : cat) {
			coord_uncert_1.push_back(make_pair(u2s.Get_RA(), u2s.Get_DE()) - Delta_coord);
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
				Delta_coord_RA[zone] += coord;
			}
		}
		//Делю на число звёзд в зоне
		for (auto& [zone, coord] : Delta_coord_RA) {
			coord /= static_cast<int>(zone_delim_RA_2[zone].size());
		}

		// 2.2 (4) stage
		for (const auto& a_1 : coord_uncert_1) {
			coord_uncert_2.push_back(
				a_1 - Delta_coord_RA[int(a_1.first)]
				);
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
				Delta_coord_DE[zone] += coord;
			}
		}
		for (auto& [zone, coord] : Delta_coord_DE) {
			coord /= static_cast<int>(zone_delim_DE_2[zone].size());
		}

		// 3.2 (6) stage
		for (const auto& a_2 : coord_uncert_2) {
			coord_uncert_3.push_back(
				a_2 - Delta_coord_DE[int(a_2.second)]
				);
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
				Delta_coord_mag[zone] += coord;
			}
		}
		for (auto& [zone, coord] : Delta_coord_mag) {
			coord /= int(zone_delim_mag[zone].size());
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
				Delta_coord_sp[zone] += coord;
			}
		}
		for (auto& [zone, coord] : Delta_coord_sp) {
			coord /= int(zone_delim_sp[zone].size());
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

	//6.2 (12) stage
	return sqrt(RA_res * RA_res + DE_res * DE_res);
}
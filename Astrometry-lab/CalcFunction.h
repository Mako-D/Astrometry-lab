#pragma once

#include "CatalogClasses.h"
#include <map>

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
	return { lhs.first / rhs, lhs.second / rhs };
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
	// �������� ����� �� �����, ��������� �� RA
		map<int, vector<pair<double, double>>> zone_delim_RA_2;
		for (const auto& [u2RAe, u2DEe] : coord_uncert_1) {
			zone_delim_RA_2[int(u2RAe)].push_back({ u2RAe, u2DEe }); // from 0 to 359 zones (totally 360)
		}

		//�������� �� Alpha_1_i
		map<int, pair<double, double>> Delta_coord_RA;
		for (const auto& [zone, star] : zone_delim_RA_2) {
			for (const auto& coord : star) {
				Delta_coord_RA[zone].first += coord.first;
				Delta_coord_RA[zone].second += coord.second;
			}
		}
		//���� �� ����� ���� � ����
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

	//6.2 (12) stage
	return sqrt(RA_res * RA_res + DE_res * DE_res);
}
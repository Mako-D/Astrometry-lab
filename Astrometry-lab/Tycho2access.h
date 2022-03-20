#pragma once

#include <utility>
#include <string>
#include <vector>

enum class Tycho2Label {
	TYC1,
	TYC2,
	TYC3,
	pflag,
	mRAdeg,
	mDEdeg,
	pmRA,
	pmDE,
	e_mRA,
	e_mDE,
	e_pmRA,
	e_pmDE,
	mepRA,
	mepDE,
	Num,
	g_mRA,
	g_mDE,
	g_pmRA,
	g_pmDE,
	BT,
	e_BT,
	VT,
	e_VT,
	prox,
	TYC,
	HIP,
	CCDM,
	RAdeg,
	DEdeg,
	epRA,
	epDE,
	e_RA,
	e_DE,
	posflg,
	corr
};

enum class UCAC2Label {
	USAS2ID,
	RA,
	DE,
	U2Rmag,
	e_RAm,
	e_DEm,
	nobs,
	e_pos,
	ncat,
	cflg,
	EpRAm,
	EpDEm,
	pmRA,
	pmDE,
	e_pmRA,
	e_pmDE,
	q_pmRA,
	q_pmDE,
	m2_id,
	m2_J,
	m2_H,
	m2_Ks,
	m2_ph,
	m2_cc
};

//std::string GetValueUCAC2_s(const std::string& str, UCAC2Label label) {
//	using namespace std;
//	pair<int, int> bit_for_mask;
//	//'d' - double
//	//'c' - char
//	//'s' - string
//	switch (label) {
//	case UCAC2Label::USAS2ID:
//		bit_for_mask = make_pair(1, 8);
//		break;
//	case UCAC2Label::RA:
//		bit_for_mask = make_pair(10, 19);
//		break;
//	case UCAC2Label::DE:
//		bit_for_mask = make_pair(21, 30);
//		break;
//	case UCAC2Label::U2Rmag:
//		bit_for_mask = make_pair(32, 36);
//		break;
//	case UCAC2Label::EpRAm:
//		bit_for_mask = make_pair(32, 36);
//		break;
//	case UCAC2Label::EpDEm:
//		bit_for_mask = make_pair(32, 36);
//		break;
//	case UCAC2Label::pmRA:
//		bit_for_mask = make_pair(32, 36);
//		break;
//	case UCAC2Label::pmDE:
//		bit_for_mask = make_pair(32, 36);
//		break;
//
//	}
//	auto [begin_mask, end_mask] = bit_for_mask;
//	return str.substr(begin_mask - 1, end_mask - begin_mask);
//}

//double GetValueUCAC2_d(const std::string& str, UCAC2Label label) {
//	return atof(GetValueUCAC2_s(str, label).c_str());
//}

//'d' - double
//'c' - char
//'s' - string
std::string GetValueTycho2_s(const std::string& str, Tycho2Label label) {
	using namespace std;
	pair<int, int> bit_for_mask;
	//'d' - double
	//'c' - char
	//'s' - string
	switch (label) {
	case Tycho2Label::TYC1:
		bit_for_mask = make_pair(1, 5);
		break;
	case Tycho2Label::TYC2:
		bit_for_mask = make_pair(6, 11);
		break;
	case Tycho2Label::TYC3:
		bit_for_mask = make_pair(12, 13);
		break;
	case Tycho2Label::pflag:
		bit_for_mask = make_pair(14, 15);
		break;
	case Tycho2Label::mRAdeg:
		bit_for_mask = make_pair(16, 28);
		break;
	case Tycho2Label::mDEdeg:
		bit_for_mask = make_pair(29, 41);
		break;
	case Tycho2Label::pmRA:
		bit_for_mask = make_pair(42, 49);
		break;
	case Tycho2Label::pmDE:
		bit_for_mask = make_pair(50, 57);
		break;
	case Tycho2Label::e_mRA:
		bit_for_mask = make_pair(58, 61);
		break;
	case Tycho2Label::e_mDE:
		bit_for_mask = make_pair(62, 65);
		break;
	case Tycho2Label::e_pmRA:
		bit_for_mask = make_pair(66, 70);
		break;
	case Tycho2Label::e_pmDE:
		bit_for_mask = make_pair(71, 75);
		break;
	case Tycho2Label::mepRA:
		bit_for_mask = make_pair(76, 83);
		break;
	case Tycho2Label::mepDE:
		bit_for_mask = make_pair(84, 91);
		break;
	case Tycho2Label::Num:
		bit_for_mask = make_pair(92, 94);
		break;
	case Tycho2Label::g_mRA:
		bit_for_mask = make_pair(95, 98);
		break;
	case Tycho2Label::g_mDE:
		bit_for_mask = make_pair(99, 102);
		break;
	case Tycho2Label::g_pmRA:
		bit_for_mask = make_pair(103, 106);
		break;
	case Tycho2Label::g_pmDE:
		bit_for_mask = make_pair(107, 110);
		break;
	case Tycho2Label::BT:
		bit_for_mask = make_pair(111, 117);
		break;
	case Tycho2Label::e_BT:
		bit_for_mask = make_pair(118, 123);
		break;
	case Tycho2Label::VT:
		bit_for_mask = make_pair(124, 130);
		break;
	case Tycho2Label::e_VT:
		bit_for_mask = make_pair(131, 136);
		break;
	case Tycho2Label::prox:
		bit_for_mask = make_pair(137, 140);
		break;
	case Tycho2Label::TYC:
		bit_for_mask = make_pair(141, 142);
		break;
	case Tycho2Label::HIP:
		bit_for_mask = make_pair(143, 148);
		break;
	case Tycho2Label::CCDM:
		bit_for_mask = make_pair(149, 152);
		break;
	case Tycho2Label::RAdeg:
		bit_for_mask = make_pair(153, 165);
		break;
	case Tycho2Label::DEdeg:
		bit_for_mask = make_pair(166, 178);
		break;
	case Tycho2Label::epRA:
		bit_for_mask = make_pair(179, 183);
		break;
	case Tycho2Label::epDE:
		bit_for_mask = make_pair(184, 188);
		break;
	case Tycho2Label::e_RA:
		bit_for_mask = make_pair(189, 194);
		break;
	case Tycho2Label::e_DE:
		bit_for_mask = make_pair(195, 200);
		break;
	case Tycho2Label::posflg:
		bit_for_mask = make_pair(201, 202);
		break;
	case Tycho2Label::corr:
		bit_for_mask = make_pair(203, 207);
		break;
	}
	auto [begin_mask, end_mask] = bit_for_mask;
	return str.substr(begin_mask - 1, end_mask - begin_mask);
}

double GetValueTycho2_d(const std::string& str, Tycho2Label label) {
	return atof(GetValueTycho2_s(str, label).c_str());
}
char GetValueTycho2_c(const std::string& str, Tycho2Label label) {
	return GetValueTycho2_s(str, label)[0];
}

//int main() {
//	using namespace std;
//	vector<double> v;
//	string input = "0001 00055 1| |  2.46593506|  2.04847203|   10.5|  -19.6| 71| 79| 1.9| 2.0|1969.12|1961.22| 3|1.3|0.6|1.3|0.6|12.405|0.217|11.638|0.149|999| |         |  2.46591139|  2.04851833|1.69|1.52| 83.2| 99.6| |-0.1\n";
//	auto start = chrono::high_resolution_clock::now();
//	for (int i = 0; i < 10'500'000; i++) {
//		v.push_back(GetValueTycho2_d(input, Tycho2Label::mDEdeg));
//		v.push_back(GetValueTycho2_d(input, Tycho2Label::DEdeg));
//		v.push_back(GetValueTycho2_d(input, Tycho2Label::BT));
//	}
//	auto end = chrono::high_resolution_clock::now();
//	chrono::duration<float> duration = end - start;
//	cout << "Duration " << duration.count() << "s" << endl;
//	system("PAUSE");
//}

//UCAC2_STAR
//{
//int32_t ra, dec;          /* RA/dec at J2000.0,  ICRS,  in milliarcsec  */
//int16_t mag;              /* internal UCAC mag,  red bandpass,  .01 mag */
//int8_t e_ra, e_dec;       /* sigmas in RA and dec at central epoch      */
//int8_t nobs;              /* Number of UCAC observations of this star   */
//int8_t e_posn;            /* Position error for UCAC observations only  */
//int8_t ncat;              /* # of catalog positions used for pmRA, pmDC */
//int8_t catalog_flag;      /* ID of major catalogs use in pmRA, pmDE     */
//int16_t epoch_ra;         /* Central epoch for mean RA, minus 1975      */
//int16_t epoch_dec;        /* Central epoch for mean DE, minus 1975      */
//int32_t pm_ra;            /* Proper motion in RA (no cos DE), mas/year  */
//int32_t pm_dec;           /* Proper motion in DE, mas/year              */
//int8_t e_pm_ra;           /* s.e. of pmRA (*cos DEm)                    */
//int8_t e_pm_dec;          /* s.e. of pmDE                               */
//int8_t q_pm_ra;           /* Goodness of fit for pmRA (in .05 units)    */
//int8_t q_pm_dec;          /* Goodness of fit for pmDE (in .05 units)    */
//int32_t twomass_id;       /* 2MASS pts_key star identifier              */
//int16_t j_mag;            /* 2MASS J  magnitude (.001 mags)             */
//int16_t h_mag;            /* 2MASS H  magnitude (.001 mags)             */
//int16_t ks_mag;           /* 2MASS Ks magnitude (.001 mags)             */
//int8_t twomass_ph_qual;   /* 2MASS modified ph_qual flag                */
//int8_t twomass_cc_flag;   /* 2MASS modified cc_flg                      */
//};
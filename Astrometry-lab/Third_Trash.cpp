#include <utility>
#include <string>

using namespace std;

enum Tycho2Label {
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

template<typename T>
T GetValueTycho2(const string& str, Tycho2Label label) {
	pair<int, int> bit_for_mask;
	//'d' - double
	//'c' - char
	char type_value = 'd';
	switch (label) {
	case Tycho2Label::TYC1:
		bit_for_mask = make_pair(1, 5);
		break;
	case Tycho2Label::TYC2:
		bit_for_mask = make_pair(6, 11);
		break;
	case Tycho2Label::TYC3:
		bit_for_mask = (12, 13);
		break;
	case Tycho2Label::pflag:
		type_value = 'c';
		bit_for_mask = (14, 15);
		break;
	case Tycho2Label::mRAdeg:
		bit_for_mask = (16, 28);
		break;
	case Tycho2Label::mDEdeg:
		bit_for_mask = (29, 41);
		break;
	case Tycho2Label::pmRA:
		bit_for_mask = (42, 49);
		break;
	case Tycho2Label::pmDE:
		bit_for_mask = (50, 57);
		break;
	case Tycho2Label::e_mRA:
		bit_for_mask = (58, 61);
		break;
	case Tycho2Label::e_mDE:
		bit_for_mask = (62, 65);
		break;
	case Tycho2Label::e_pmRA:
		bit_for_mask = (66, 70);
		break;
	case Tycho2Label::e_pmDE:
		bit_for_mask = (71, 75);
		break;
	case Tycho2Label::mepRA:
		bit_for_mask = (76, 83);
		break;
	case Tycho2Label::mepDE:
		bit_for_mask = (84, 91);
		break;
	case Tycho2Label::Num:
		bit_for_mask = (92, 94);
		break;
	case Tycho2Label::g_mRA:
		bit_for_mask = (95, 98);
		break;
	case Tycho2Label::g_mDE:
		bit_for_mask = (99, 102);
		break;
	case Tycho2Label::g_pmRA:
		bit_for_mask = (103, 106);
		break;
	case Tycho2Label::g_pmDE:
		bit_for_mask = (107, 110);
		break;
	case Tycho2Label::BT:
		bit_for_mask = (111, 117);
		break;
	case Tycho2Label::e_BT:
		bit_for_mask = (118, 123);
		break;
	case Tycho2Label::VT:
		bit_for_mask = (124, 130);
		break;
	case Tycho2Label::e_VT:
		bit_for_mask = (131, 136);
		break;
	case Tycho2Label::prox:
		bit_for_mask = (137, 140);
		break;
	case Tycho2Label::TYC:
		type_value = 'c';
		bit_for_mask = (141, 142);
		break;
	case Tycho2Label::HIP:
		bit_for_mask = (143, 148);
		break;
	case Tycho2Label::CCDM:
		bit_for_mask = (149, 152);
		break;
	case Tycho2Label::RAdeg:
		bit_for_mask = (153, 165);
		break;
	case Tycho2Label::DEdeg:
		bit_for_mask = (166, 178);
		break;
	case Tycho2Label::epRA:
		bit_for_mask = (179, 183);
		break;
	case Tycho2Label::epDE:
		bit_for_mask = (184, 188);
		break;
	case Tycho2Label::e_RA:
		bit_for_mask = (189, 194);
		break;
	case Tycho2Label::e_DE:
		bit_for_mask = (195, 200);
		break;
	case Tycho2Label::posflg:
		type_value = 'c';
		bit_for_mask = (201, 202);
		break;
	case Tycho2Label::corr:
		bit_for_mask = (203, 206);
		break;
	}
	[begin_mask, end_mask] = bit_for_bask;
	string result = substr(begin_mask - 1, end_mask - begin_mask);
}
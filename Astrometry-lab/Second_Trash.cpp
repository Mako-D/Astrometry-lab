#include <iostream>
#include <cmath>

using namespace std;

/* Область определения cos -> 0 - 360 градусов
* Область определение sin -> -180 - 180 градусов
* По умолчанию тип угла cos.
* Класс хранит в себе угол типа cos. В тех моментах,
* когда становится критичным, какой тип угла взят,
* учитывается введённый тип
*/

#define AngleSecond (1. / (60 * 60));
#define AngleMinute (1. / 60);

enum AngleType {
	fullcos,
	halfsin
};

class Angle {
	double deg;
	AngleType type = AngleType::fullcos;

	template<typename T>
	T round_angle(const T& angle) {
		switch (type) {
		case AngleType::fullcos:
			if (angle < 0) {
				return angle + 360;
			}
			else if (angle > 360) {
				return fmod(angle, 360);
			}
			else {
				return angle;
			}
			break;
		case AngleType::halfsin:
			if (angle < -90) {
				return -90 - fmod(angle,90);
			}
			else if (angle > 90) {
				return 90 - fmod(angle, 90);
			}
			else {
				return angle;
			}
			break;
		}
	}
	Angle() {
		this->deg = 0;
		this->type = AngleType::fullcos;
	}
	Angle(double deg, AngleType type = AngleType::fullcos) {
		this->type = type;
		this->deg = round_angle(deg);
	}
	friend Angle operator+ (const Angle& lhs, const Angle& rhs) {
		if (lhs.type == rhs.type) {
			return{ lhs.deg + rhs.deg , lhs.type};
		}
		else {
			cout << "UNDIFINED BEHAVIOR IN OPERATOR+, class Angle";
			exit(1);
		}
	}

};

bool AreEqualAngles(const Angle& lhs, const Angle& rhs) {
	return abs(lhs.deg - rhs.deg) < AngleSecond;
}

int main() {
	Angle a(350.255);
	Angle b(30.123);
	bool c = AreEqualAngles({ 2.5555 }, { 2.555556 });
	
	return 0;
}
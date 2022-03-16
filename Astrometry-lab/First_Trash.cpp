#include <iostream>
#include <cmath>
#include <tuple>

using namespace std;

/* Область определения cos -> 0 - 360 градусов
* Область определение sin -> -180 - 180 градусов
* По умолчанию тип угла cos.
* Класс хранит в себе угол типа cos. В тех моментах,
* когда становится критичным, какой тип угла взят,
* учитывается введённый тип
*/
enum AngleType{
	fullcos,
	halfsin
};

class Angle {
private:
	int deg = 0;
	int min = 0;
	double sec = 0.;
	AngleType type = AngleType::fullcos;

	template<typename T>
	T round_angle(const T& angle) {
		if (type == AngleType::fullcos) {
			if (angle < 0) {
				return angle + 360;
			}
			else if (angle > 360) {
				return fmod(angle, 360);
			}
			else {
				return angle;
			}
		}
	}
public:
	

	Angle() {
		this->deg = this->min = 0;
		this->sec = 0.;
		AngleType type = AngleType::fullcos;
	}
	Angle(double deg, AngleType type = AngleType::fullcos) {

		double fractional_part_of_deg;
		double whole_part_of_deg;
		fractional_part_of_deg = modf(round_angle(deg), &whole_part_of_deg);
		this->deg = int(whole_part_of_deg);

		double fractional_part_of_min;
		double whole_part_of_min;
		fractional_part_of_min = modf(fractional_part_of_deg * 60., &whole_part_of_min);
		this->min = int(whole_part_of_min);

		this->sec = fractional_part_of_min*60;
	}

	Angle(int deg, int min, double sec, AngleType type = AngleType::fullcos) {

		this->deg = round_angle(deg);
		this->min = min;
		this->sec = sec;
	}

	friend Angle operator+ (const Angle& lhs, const Angle& rhs) {

		double sec_new = fmod(lhs.sec + rhs.sec, 60);
		int min_additional = int((lhs.sec + rhs.sec) / 60);
		
		int min_new = int(fmod(lhs.min + rhs.min + min_additional, 60));
		int deg_additional = (lhs.min + rhs.min + min_additional) / 60;

		int deg_new = int(fmod(lhs.deg + rhs.deg + deg_additional, 360));

		return { deg_new, min_new, sec_new };
	}

};
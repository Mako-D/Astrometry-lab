#include <iostream>
#include <cmath>


/* Область определения cos -> 0 - 360 градусов
* Область определение sin -> -180 - 180 градусов
* По умолчанию тип угла cos.
* Класс хранит в себе угол типа cos. В тех моментах,
* когда становится критичным, какой тип угла взят,
* учитывается введённый тип
*/

#define AngleSecond 1. / (60. * 60.); //Секунда угловой дуги
#define AngleMinute double(1. / 60.); //Минута угловой дуги
#define AngleMAS double(1./(60. * 60. * 100.)) //Милисекунда угловой дуги
#define AngleAccuracy AngleSecond // Точность угла
#define MagnitudeAccuracy 1.

enum AngleType {
	fullcos,
	halfsin
};


class Angle {
private:
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
				return -90 - fmod(angle, 90);
			}
			else if (angle > 90) {
				return 90 - fmod(angle, 90);
			}
			else {
				return angle;
			}
			break;
		}
		return angle;
	}
public:
	Angle() {
		this->deg = 0;
		this->type = AngleType::fullcos;
	}
	Angle(double deg, AngleType type = AngleType::fullcos) {
		this->type = type;
		this->deg = round_angle(deg);
	}

	double GetDeg() const {
		return deg;
	}
	AngleType GetType() const {
		return type;
	}
	friend Angle operator+ (const Angle& lhs, const Angle& rhs) {
		if (lhs.type == rhs.type) {
			return{ lhs.deg + rhs.deg , lhs.type };
		}
		else {
			std::cout << "UNDIFINED BEHAVIOR IN OPERATOR+, class Angle";
			exit(1);
		}
	}
	friend Angle operator+ (const Angle& lhs, double rhs) {
		return { lhs.deg + rhs, lhs.type };
	}
	friend bool operator==(const Angle& lhs, const Angle& rhs) {
		return abs(lhs.deg - rhs.deg) < AngleAccuracy;
	}

};

class Coordinates {
private:
	Angle alpha;
	Angle delta;
public:
	Coordinates() {
		this->alpha = { 0., AngleType::fullcos };
		this->delta = { 0., AngleType::halfsin };
	}
	Coordinates(double alpha, double delta) {
		this->alpha = { alpha, AngleType::fullcos };
		this->delta = { delta, AngleType::halfsin };
	}
	friend bool operator == (const Coordinates& lhs, const Coordinates& rhs) {
		return (lhs.alpha == rhs.alpha) &&
			   (lhs.delta == rhs.delta);
	}
	void MoveStar(double speed_in_mas_alpha, double speed_in_mas_delta, double time_in_year) {
		alpha = alpha + speed_in_mas_alpha * AngleMAS * time_in_year;
		delta = delta + speed_in_mas_delta * AngleMAS * time_in_year;
	}
	double Get_alpha() const{
		return alpha.GetDeg();
	}
	double Get_delta() const {
		return delta.GetDeg();
	}
	friend std::istream& operator << (std::istream& is, Coordinates& c) {
		double a, d;
		is >> a >> d;
		c = { a, d };
		return is;
	}

};

//int main() {
//	Angle a(350.255);
//	Angle b(30.123);
//	bool c = a == b;
//	
//	return 0;
//}
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <string>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <chrono>

extern "C" {
#include "ucac2.h"
}
#include "CalcFunction.h"

#define AreaAccuraccyDeg 3. * 1. / (60. * 60.)

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
	system("pause");
	return 0;
}
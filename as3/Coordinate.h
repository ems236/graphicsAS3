#pragma once

#include <vector>

using namespace std;

class Coordinate
{
public:
	static Coordinate point3(double x, double y, double z);
	static Coordinate vector3(double x, double y, double z);
	static Coordinate point3_non_homogeneous(double x, double y, double z);

	void print();
	double x();
	double y();
	double z();

	void homogenize(bool isPoint);
	vector<double> data;
	bool is_homogeneous;
	int length();

	Coordinate normalized();

	double operator *(Coordinate& other);

	//Cross product
	Coordinate operator &(Coordinate& other);
private:
	static Coordinate build3(double x, double y, double z, bool isPoint, bool is_homogeneous);
};
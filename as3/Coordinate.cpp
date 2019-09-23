#include <iostream>
#include "Coordinate.h"

Coordinate Coordinate::build3(double x, double y, double z, bool isPoint, bool is_homogeneous = true)
{
	Coordinate point = *new Coordinate();
	point.data.push_back(x);
	point.data.push_back(y);
	point.data.push_back(z);
	if (is_homogeneous)
	{
		point.homogenize(isPoint);
	}

	return point;
}

Coordinate Coordinate::point3(double x, double y, double z)
{
	return build3(x, y, z, true);
}

Coordinate Coordinate::vector3(double x, double y, double z)
{
	return build3(x, y, z, false);
}

Coordinate Coordinate::point3_non_homogeneous(double x, double y, double z)
{
	return build3(x, y, z, false, false);
}

void Coordinate::print()
{
	cout << "[";
	for (int i = 0; i < data.size(); i++)
	{
		cout << data.at(i) << " ";
	}
	cout << "]" << endl;
}

int Coordinate::length()
{
	return data.size();
}

double Coordinate::x()
{
	return length() >= 1 ? data.at(0) : 0;
}

double Coordinate::y()
{
	return length() >= 2 ? data.at(1) : 0;
}

double Coordinate::z()
{
	return length() >= 3 ? data.at(2) : 0;
}

void Coordinate::homogenize(bool isPoint = true)
{
	double to_add = isPoint ? 1 : 0;

	if (is_homogeneous)
	{
		data.back() = to_add;
	}

	else
	{
		data.push_back(to_add);
		is_homogeneous = true;
	}
}

Coordinate Coordinate::normalized()
{
	double sum = 0;
	int size = is_homogeneous ? data.size() - 1 : data.size();
	for (int i = 0; i < size; i++)
	{
		sum += (data.at(i) * data.at(i));
	}

	double rootSum = sqrt(sum);
	Coordinate result = *new Coordinate();

	for (int i = 0; i < size; i++)
	{
		result.data.push_back(data.at(i) / rootSum);
	}

	result.homogenize(false);

	return result;
}

double Coordinate::operator *(Coordinate& other)
{
	if (length() != other.length())
	{
		throw new exception("Length mismatch");
	}

	double sum = 0;
	for (int i = 0; i < data.size(); i++)
	{
		sum += data.at(i) * other.data.at(i);
	}

	return sum;
}

//Cross product
Coordinate Coordinate::operator &(Coordinate& other)
{
	if (!(length() == 3 || (is_homogeneous && length() == 4)) || !(other.length() == 3 || (other.is_homogeneous && other.length() == 4)))
	{
		throw new exception("cross product only works for length 3");
	}

	double x1 = data.at(0);
	double y1 = data.at(1);
	double z1 = data.at(2);

	double x2 = other.data.at(0);
	double y2 = other.data.at(1);
	double z2 = other.data.at(2);

	double newx = y1 * z2 - y2 * z1;
	double newy = x2 * z1 - x1 * z2;
	double newz = x1 * y2 - x2 * y1;

	return Coordinate::vector3(newx, newy, newz);
}
#pragma once
#include "Coordinate.h";

class Matrix
{
public:
	static Matrix rotation_x(double theta);
	static Matrix rotation_y(double theta);
	static Matrix rotation_z(double theta);
	static Matrix translation(Coordinate vector);
	static Matrix scale(double factor);

	int rows;
	void print();
	vector<Coordinate> data;
	void add(Coordinate column);
	int columns();
	Coordinate row(int index);

	Coordinate operator *(Coordinate& other);
	Matrix operator *(Matrix&
		other);
};
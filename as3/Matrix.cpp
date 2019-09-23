#include "Matrix.h";


Matrix Matrix::rotation_z(double theta)
{
	double sin_angle = sin(theta);
	double cos_angle = cos(theta);

	Matrix rotation = *new Matrix();

	Coordinate first_column = Coordinate::vector3(cos_angle, sin_angle, 0);
	Coordinate second_column = Coordinate::vector3(-1 * sin_angle, cos_angle, 0);
	Coordinate third_column = Coordinate::vector3(0, 0, 1);
	Coordinate fourth_column = Coordinate::point3(0, 0, 0);

	rotation.add(first_column);
	rotation.add(second_column);
	rotation.add(third_column);
	rotation.add(fourth_column);

	return rotation;
}

Matrix Matrix::rotation_y(double theta)
{
	double sin_angle = sin(theta);
	double cos_angle = cos(theta);

	Matrix rotation = *new Matrix();

	Coordinate first_column = Coordinate::vector3(cos_angle, 0, -1 * sin_angle);
	Coordinate second_column = Coordinate::vector3(0, 1, 0);
	Coordinate third_column = Coordinate::vector3(sin_angle, 0, cos_angle);
	Coordinate fourth_column = Coordinate::point3(0, 0, 0);

	rotation.add(first_column);
	rotation.add(second_column);
	rotation.add(third_column);
	rotation.add(fourth_column);

	return rotation;
}

Matrix Matrix::rotation_x(double theta)
{
	double sin_angle = sin(theta);
	double cos_angle = cos(theta);

	Matrix rotation = *new Matrix();

	Coordinate first_column = Coordinate::vector3(1, 0, 0);
	Coordinate second_column = Coordinate::vector3(0, cos_angle, sin_angle);
	Coordinate third_column = Coordinate::vector3(0, -1 * sin_angle, cos_angle);
	Coordinate fourth_column = Coordinate::point3(0, 0, 0);

	rotation.add(first_column);
	rotation.add(second_column);
	rotation.add(third_column);
	rotation.add(fourth_column);

	return rotation;
}

Matrix Matrix::translation(Coordinate vector)
{
	vector.homogenize(true);
	Matrix translation = *new Matrix();

	Coordinate first_column = Coordinate::vector3(1, 0, 0);
	Coordinate second_column = Coordinate::vector3(0, 1, 0);
	Coordinate third_column = Coordinate::vector3(0, 0, 1);



	translation.add(first_column);
	translation.add(second_column);
	translation.add(third_column);
	translation.add(vector);

	return translation;
}

Matrix Matrix::scale(double factor)
{
	Matrix scale = *new Matrix();

	Coordinate first_column = Coordinate::vector3(factor, 0, 0);
	Coordinate second_column = Coordinate::vector3(0, factor, 0);
	Coordinate third_column = Coordinate::vector3(0, 0, factor);
	Coordinate fourth_column = Coordinate::point3 (0, 0, 0);


	scale.add(first_column);
	scale.add(second_column);
	scale.add(third_column);
	scale.add(fourth_column);

	return scale;
}

void Matrix::print()
{
	for (int i = 0; i < rows; i++)
	{
		row(i).print();
	}
}

void Matrix::add(Coordinate column)
{
	if (data.size() == 0)
	{
		rows = column.length();
	}

	if (rows != column.length())
	{
		throw new exception("bad column");
	}

	data.push_back(column);
}

int Matrix::columns()
{
	return data.size();
}

Coordinate Matrix::row(int index)
{
	Coordinate* row = new Coordinate();
	for (vector<Coordinate>::iterator it = data.begin(); it != data.end(); ++it) {
		row->data.push_back(it->data.at(index));
	}

	return *row;
}

Coordinate Matrix::operator *(Coordinate& other)
{
	if (columns() != other.length())
	{
		throw new exception("length mismatch");
	}

	Coordinate* product = new Coordinate();
	product->is_homogeneous = true;
	for (int i = 0; i < rows; i++)
	{
		Coordinate current_row = row(i);
		double first = current_row * other;
		product->data.push_back(first);
		//		delete(&current_row);
	}

	return *product;
}

Matrix Matrix::operator *(Matrix& other)
{
	if (columns() != other.rows)
	{
		throw new exception("length mismatch");
	}

	Matrix* product = new Matrix();

	for (int i = 0; i < columns(); i++)
	{
		Coordinate product_column = *this * other.data.at(i);
		product->add(product_column);
	}

	return *product;
}
#include "gri/Vector2D.h"


namespace gri
{

Vector2D::Vector2D()
{
	x = 0.0;
	y = 0.0;
}

Vector2D::Vector2D(double x_p, double y_p)
{
	x = x_p;
	y = y_p;
}

Vector2D::~Vector2D()
{
	
}

double Vector2D::getX()
{
	return x;
}

double Vector2D::getY()
{
	return y;
}

void Vector2D::setX(double value)
{
	x = value;
}

void Vector2D::setY(double value)
{
	y = value;
}

Vector2D& Vector2D::operator=(Vector2D param)
{
	if(this != &param)
	{
		x = param.getX();
		y = param.getY();
	}
	return *this;
}

Vector2D Vector2D::operator+(Vector2D param)
{
	Vector2D temp;
	temp.setX(x + param.getX());
	temp.setY(y + param.getY());
	return temp;
}

Vector2D Vector2D::operator-(Vector2D param)
{
	Vector2D temp;
	temp.setX(x - param.getX());
	temp.setY(y - param.getY());
	return temp;
}


}






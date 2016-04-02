#pragma once


namespace gri
{

class Vector2D
{
	public:
		Vector2D();
		Vector2D(double x_p, double y_p);
		~Vector2D();
		Vector2D& operator= (Vector2D param);
		Vector2D operator+ (Vector2D param);
		Vector2D operator- (Vector2D param);
		double getX();
		double getY();
		void setX(double value);
		void setY(double value);
	private:
		double x;
		double y;
};



}



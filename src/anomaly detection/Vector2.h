#pragma once

#ifndef VECTOR2_H_
#define VECTOR2_H_
#include <ostream>
#include <sstream>
#include <cmath>
class Vector2
{
public:
	//Members
	float X;
	float Y;
	//Constructors
	Vector2();
	explicit Vector2(float const&newX, float const&newY);
	//Math
	//Vector3 by Vector3 Math
	Vector2		operator+(const Vector2& vector) const;
	Vector2		operator-(const Vector2& vector) const;
	Vector2		operator*(const Vector2& vector) const;
	Vector2		operator/(const Vector2& vector) const;
	Vector2&	operator+=(const Vector2& vector);
	Vector2&	operator-=(const Vector2& vector);
	Vector2&	operator*=(const Vector2& vector);
	Vector2&	operator/=(const Vector2& vector);
	//Vector3 by Float Math
	Vector2		operator-() const;
	Vector2		operator+(float const&num) const;
	Vector2		operator-(float const&num) const;
	Vector2		operator*(float const&num) const;
	Vector2		operator/(float const&num) const;
	Vector2&	operator+=(float const&num);
	Vector2&	operator-=(float const&num);
	Vector2&	operator*=(float const&num);
	Vector2&	operator/=(float const&num);
	//Boolean Equals Operators
	bool		operator==(const Vector2& vector) const;
	bool		operator!=(const Vector2& vector) const;
	//Print out
	friend std::ostream& operator<< (std::ostream& ofs, const Vector2& vector);
};

//Constructors
inline Vector2::Vector2() {}
inline Vector2::Vector2(float const&x, float const&y) : X(x), Y(y) {}

// Math
inline Vector2 Vector2::operator+(const Vector2& vector) const
{
	return Vector2(X + vector.X, Y + vector.Y);
}

inline Vector2 Vector2::operator-(const Vector2& vector) const
{
	return Vector2(X - vector.X, Y - vector.Y);
}
inline Vector2 Vector2::operator*(const Vector2& vector) const
{
	return Vector2(X * vector.X, Y * vector.Y);
}
inline Vector2 Vector2::operator/(const Vector2& vector) const
{
	return Vector2(X / vector.X, Y / vector.Y);
}
inline Vector2& Vector2::operator+=(const Vector2& vector)
{
	X += vector.X;
	Y += vector.Y;
	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& vector)
{
	X -= vector.X;
	Y -= vector.Y;
	return *this;
}
inline Vector2& Vector2::operator*=(const Vector2& vector)
{
	X *= vector.X;
	Y *= vector.Y;
	return *this;
}

inline Vector2& Vector2::operator/=(const Vector2& vector)
{
	X /= vector.X;
	Y /= vector.Y;
	return *this;
}

inline Vector2 Vector2::operator-() const
{
	return Vector2(-X, -Y);
}
inline Vector2 Vector2::operator-(float const&num) const
{
	return Vector2(X - num, Y - num);
}
inline Vector2 Vector2::operator+(float const&num) const
{
	return Vector2(X + num, Y + num);
}
inline Vector2 Vector2::operator*(float const&num) const
{
	return Vector2(X * num, Y * num);
}

inline Vector2 Vector2::operator/(float const&num) const
{
	return Vector2(X / num, Y / num);
}
inline Vector2& Vector2::operator+=(const float& num)
{
	X += num;
	Y += num;
	return *this;
}

inline Vector2& Vector2::operator-=(const float& num)
{
	X -= num;
	Y -= num;
	return *this;
}
inline Vector2& Vector2::operator*=(const float& num)
{
	X *= num;
	Y *= num;
	return *this;
}

inline Vector2& Vector2::operator/=(const float& num)
{
	X /= num;
	Y /= num;
	return *this;
}
//Boolean
inline bool Vector2::operator==(const Vector2& vector) const
{
	return X == vector.X && Y == vector.Y;
}

inline bool Vector2::operator!=(const Vector2& vector) const
{
	return X != vector.X || Y != vector.Y;
}
//Subtractes the two Vector2 objects
inline const Vector2 operator-(Vector2 const&lhs, Vector2 const&rhs) {
	return Vector2(lhs.X - rhs.X, lhs.Y - rhs.Y);
}
//Adds the two Vector2 objects
inline const Vector2 operator+(Vector2 const&lhs, Vector2 const&rhs) {
	return Vector2(lhs.X + rhs.X, lhs.Y + rhs.Y);
}
//divides the two Vector2 objects
inline const Vector2 operator/(Vector2 const&lhs, float const&rhs) {
	return Vector2(lhs.X / rhs, lhs.Y / rhs);
}
//Multiplies the two Vector2 objects
inline const Vector2 operator*(float lhs, Vector2 const &rhs) {
	Vector2 result;
	result.X = rhs.X * lhs;
	result.Y = rhs.Y * lhs;
	return result;
}
//Subtractes the two Vector2 objects
inline const Vector2& operator-=(Vector2 &lhs, const Vector2 &rhs) {
	lhs.X -= rhs.X;
	lhs.Y -= rhs.Y;
	return lhs;
}
//Adds the two Vector2 objects
inline const Vector2& operator+=(Vector2 &lhs, const Vector2 &rhs) {
	lhs.X += rhs.X;
	lhs.Y += rhs.Y;
	return  lhs;
}
//Divides the two Vector2 objects
inline const Vector2& operator/=(Vector2 &lhs, const Vector2 &rhs) {
	lhs.X /= rhs.X;
	lhs.Y /= rhs.Y;
	return  lhs;
}
//Multiplies the two Vector2 objects
inline const Vector2& operator*=(Vector2 &lhs, const Vector2 &rhs) {
	lhs.X *= rhs.X;
	lhs.Y *= rhs.Y;
	return  lhs;
}
//Multiplies the Vector2 object by a float
inline const Vector2& operator*=(Vector2 &lhs, const float &rhs) {
	lhs.X *= rhs;
	lhs.Y *= rhs;
	return  lhs;
}
//Compares the two Vector2 objects and returns (true) if they are not equal
inline const bool operator!=(Vector2 const&lhs, Vector2 const&rhs) {
	return (lhs.X != rhs.X) || (lhs.Y != rhs.Y);
}
//Compares the two Vector2 objects and returns (true) if they are equal
inline const bool operator==(Vector2 const&lhs, Vector2 const&rhs) {
	return ((lhs.X * rhs.X < (float)0.000001) && (lhs.Y * rhs.Y < (float)0.000001));
}

//Sets the Vector2 with zero values
inline void zero(Vector2 &lhs) {
	lhs.X = 0.0f;
	lhs.Y = 0.0f;
}
//Returns if the Vector2 is zero
inline const bool isZero(Vector2 const&lhs) {
	return (lhs.X*lhs.X + lhs.Y*lhs.Y) < (float)0.000001;//EPSILON
}
//Returns dot product
inline const float Dot(Vector2 const&lhs, Vector2 const&rhs) {
	return lhs.X*rhs.X + lhs.Y*rhs.Y;
}
//Returns length squared
inline const float SqrMagnitude(Vector2 const&rhs) {
	return Dot(rhs, rhs);
}
//Returns magnitude (length)
inline const float Magnitude(Vector2 const &rhs) {
	//return sqrtf(dot(rhs, rhs));
	return sqrt(Dot(rhs, rhs));
}
//Returns normalized Vector2
inline Vector2 Normalize(Vector2 const &lhs) {
	return (1.f / (Magnitude(lhs))) * lhs;
}
//Returns cross product for Vector2
inline float Cross(Vector2 const &lhs, Vector2 const &rhs) {
	return (lhs.X * rhs.Y - lhs.Y * rhs.X);
}
//Returns (Vector2) cross product of the Vector2 objects
inline Vector2 CrossVector(Vector2 const &vec2, Vector2 const &vec1) {
	return Vector2(
		vec2.Y * vec1.X - vec1.Y * vec2.X,
		vec2.X * vec1.Y - vec1.X * vec2.Y);
}
//Returns Negative or Positive (value of one)
inline const int sign(const Vector2& lhs, const Vector2& rhs) {
	if (lhs.Y*rhs.X > lhs.X*rhs.Y)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
//Returns perpendicular Vector2
inline const Vector2 Orthogonal(Vector2 const &rhs) {
	return Vector2(-rhs.Y, rhs.X);
}
//Clamps Vector2 values to max
inline void Truncate(Vector2 &rhs, float const& max) {
	if (Magnitude(rhs) > max)
	{
		Normalize(rhs);
		rhs = rhs * max;
	}
}
//Returns inverse (negative) Vector2
inline Vector2 Inverse(Vector2 const &rhs) {
	return Vector2(-rhs.X, -rhs.Y);
}
//Returns distance between the Vector2's
inline double Distance(Vector2 const&lhs, Vector2 const&rhs) {
	double ySeparation = rhs.Y - lhs.Y;
	double xSeparation = rhs.X - lhs.X;

	//return sqrtf(ySeparation*ySeparation + xSeparation*xSeparation);
	return sqrt(ySeparation*ySeparation + xSeparation * xSeparation);
}
//Returns squared distance between the Vector2's
inline double DistanceSquared(Vector2 const&lhs, Vector2 const&rhs) {
	double ySeparation = rhs.Y - lhs.Y;
	double xSeparation = rhs.X - lhs.X;

	return ySeparation * ySeparation + xSeparation * xSeparation;
}
//Returns the lhs Vector2 reflected over the rhs Vector2
inline Vector2 Reflect(Vector2 const&lhs, Vector2 const&rhs) {
	Vector2 out;
	const float dotProductTimesTwo = Dot(rhs, lhs) * 2.0f;
	out.X = rhs.X - (dotProductTimesTwo * lhs.X);
	out.Y = rhs.Y - (dotProductTimesTwo * lhs.Y);
	return out;
}
//Output display of values
inline std::ostream& operator<< (std::ostream& os, const Vector2& vector)
{
	std::stringstream stream;
	stream << "X: " << vector.X << ", Y: " << vector.Y;
	os.write(const_cast<char*>(stream.str().c_str()), static_cast<std::streamsize>(stream.str().size() * sizeof(char)));
	return os;
}

#endif /* VECTOR2_H_ */

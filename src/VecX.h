#ifndef VECX_H
#define VECX_H

#include <cmath>

namespace t3 {

/// 2D vector template class, defines a few standard operations
template <typename T> class Vec2 {
public:
	Vec2() : x(0),y(0) {}
	Vec2(T x, T y) {this->x = x; this->y = y;}

	/// Element-wise vector addition
	Vec2<T> operator+( const Vec2<T> &rhs ) const {
		return Vec2<T>(x + rhs.x, y + rhs.y);
	}

	/// Scalar division
	Vec2<T> operator/( T s ) const {
		return Vec2<T>(x / s, y / s);
	}

	T x, y;
};

/// 3D vector template class, defines a few standard operations
template <typename T> class Vec3 {
public:
	Vec3() : x(0),y(0),z(0) {}
	Vec3(T x, T y, T z) {this->x = x; this->y = y; this->z = z;}
	T x, y, z;

	/// Element-wise vector addition
	Vec3<T> operator+( const Vec3<T> &rhs ) const {
		return Vec3<T>(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	/// Scalar division
	Vec3<T> operator/( T s ) {
		return Vec3<T>(x / s, y / s, z / s);
	}

	/// Performs an angular transform on the vector, about the origin.
	/// \param r A vector of 3 angles (Euler) in radians.
	Vec3<T> AngularTransform( Vec3<T> &r ) {
		return Vec3<T>(
			(cos(r.z)*cos(r.y)+sin(r.z)*sin(r.x)*sin(r.y))*x + (-cos(r.z)*sin(r.y)+sin(r.z)*sin(r.x)*cos(r.y))*y +(sin(r.z)*cos(r.x))*z,
			(sin(r.y)*cos(r.x))*x + (cos(r.y)*cos(r.x))*y + (-sin(r.x))*z,
			(-sin(r.z)*cos(r.y)+cos(r.z)*sin(r.x)*sin(r.y))*x + (sin(r.y)*sin(r.z)+cos(r.z)*sin(r.x)*cos(r.y))*y + (cos(r.z)*cos(r.x))*z
			);
	}

	/// Computes the 3D dot-product against another vector
	float DotProduct( Vec3<T> &v ) {
		return x*v.x+y*v.y+z*v.z;
	}
};

}; // namespace t3

#endif
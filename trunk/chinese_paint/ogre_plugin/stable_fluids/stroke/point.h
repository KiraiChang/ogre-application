#ifndef _POINT_H_
#define _POINT_H_
#include <math.h>
#include <list>
#include <vector>
#include <assert.h>
namespace Stroke
{
	class Point
	{
	public:
		float x, y;
	public:
		Point(float _x, float _y):x(_x), y(_y){}
		Point(){Point(0, 0);}
		~Point(){}
		// arithmetic operations
		inline Point operator + ( const Point& rkVector ) const
		{
			return Point(
				x + rkVector.x,
				y + rkVector.y);
		}

		inline Point operator - ( const Point& rkVector ) const
		{
			return Point(
				x - rkVector.x,
				y - rkVector.y);
		}

		inline Point operator * ( const float fScalar ) const
		{
			return Point(
				x * fScalar,
				y * fScalar);
		}

		inline Point operator * ( const Point& rhs) const
		{
			return Point(
				x * rhs.x,
				y * rhs.y);
		}

		inline friend Point operator * ( const float fScalar, const Point& rkVector )
		{
			return Point(
				fScalar * rkVector.x,
				fScalar * rkVector.y);
		}

		inline Point& operator = ( const Point& rkVector )
		{
			x = rkVector.x;
			y = rkVector.y;

			return *this;
		}

		inline Point& operator += ( const Point& rkVector )
		{
			x += rkVector.x;
			y += rkVector.y;

			return *this;
		}

		inline Point& operator += ( const float fScaler )
		{
			x += fScaler;
			y += fScaler;

			return *this;
		}

		inline Point& operator -= ( const Point& rkVector )
		{
			x -= rkVector.x;
			y -= rkVector.y;

			return *this;
		}

		inline Point& operator -= ( const float fScaler )
		{
			x -= fScaler;
			y -= fScaler;

			return *this;
		}

		inline Point& operator *= ( const float fScaler )
		{
			x *= fScaler;
			y *= fScaler;

			return *this;
		}

		inline Point& operator /= ( const Point& rkVector )
		{
			x /= rkVector.x;
			y /= rkVector.y;

			return *this;
		}

		inline Point& operator /= ( const float fScalar )
		{
			assert( fScalar != 0.0 );

			float fInv = 1.0f / fScalar;

			x *= fInv;
			y *= fInv;

			return *this;
		}

		inline bool operator < (const Stroke::Point &rhs)
		{
			return (x < rhs.x) || (x == rhs.x && y < rhs.y);
		}

		inline float length () const
		{
			return sqrt( x * x + y * y );
		}

		inline float distance(const Point& rhs) const
		{
			return (*this - rhs).length();
		}

		inline float normalise()
		{
			float fLength = sqrt( x * x + y * y);

			// Will also work for zero-sized vectors, but will change nothing
			if ( fLength > 1e-08 )
			{
				float fInvLength = 1.0f / fLength;
				x *= fInvLength;
				y *= fInvLength;
			}

			return fLength;
		}

		inline Point normalisedCopy(void) const
		{
			Point ret = *this;
			ret.normalise();
			return ret;
		}

		inline float dot(const Point &rh) const
		{
			return x * rh.x + y * rh.y;
		}
	};
	typedef std::list < Point >		LIST_POINT;
	typedef std::vector < Point >	V_POINT;
}

#endif //_POINT_H_
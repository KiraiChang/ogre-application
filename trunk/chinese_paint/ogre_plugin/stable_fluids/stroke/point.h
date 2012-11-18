#ifndef _POINT_H_
#define _POINT_H_
#include <math.h>
#include <list>
#include <vector>
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
	};
	typedef std::list < Point >		LIST_POINT;
	typedef std::vector < Point >	V_POINT;
}

#endif //_POINT_H_
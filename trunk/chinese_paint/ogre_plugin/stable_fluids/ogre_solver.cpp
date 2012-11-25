#include <ogre.h>
#include "stroke\point.h"

#define CROSS(o, a, b) (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x)

Stroke::V_POINT convex_hull(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber, float threshold)
{
	int i, x, y, t;
	Stroke::Point p1;
	Stroke::Point p2;
	Stroke::Point dist;
	Stroke::V_POINT vPoint;
	Stroke::V_POINT vResult;
	for(i = 0;i < verticesCount;i++)
	{
		if(vertices[i].y < threshold && vertices[i].y > -threshold)//the y position at surface
		{
			p1.x = vertices[i].x;

			p1.y = vertices[i].z;

			vPoint.push_back(p1);
		}
	}

	// 將所有點依照座標大小排序
	std::sort(vPoint.begin(), vPoint.end());

	int m = 0;  // m 為凸包頂點數目

	size_t size = vPoint.size();
	vResult.resize(size * 2);
	// 包下半部
	for (i=0; i<size; ++i)
	{
		while (m >= 2 && CROSS(vResult[m-2], vResult[m-1], vPoint[i]) <= 0) m--;
		vResult[m++] = vPoint[i];
	}

	// 包上半部，不用再包入方才包過的終點，但會再包一次起點
	for (i=size-2, t=m+1; i>=0; --i)
	{
		while (m >= t && CROSS(vResult[m-2], vResult[m-1], vPoint[i]) <= 0) m--;
		vResult[m++] = vPoint[i];
	}

	m--;    // 最後一個點是重複出現兩次的起點，故要減一。
	vResult.resize(m);

	return vResult;
}
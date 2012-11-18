#include "mapping_algorithm.h"


Stroke::V_POINT Normal::process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber)
{
	Stroke::V_POINT result;
	int gridSize = (gridNumber+2)*(gridNumber+2);
	int i, x, y, ite;
	unsigned int index;
	bool hasEdge = false;
	int front, back;
	for(i = 0;i < verticesCount;i++)
	{
		if(vertices[i].y < 1.0 && vertices[i].y > -1.0)//the y position at surface
		{
			x = ((int)vertices[i].x);

			y = ((int)vertices[i].z);

			index = x +  (y*(gridNumber+2));
			if(index < gridSize)
				grid[index] = 1.0;//draw contour of object
		}
	}

	for(y = 1; y < gridNumber; y++)//fill contour hole
	{
		hasEdge = false;
		for(x = 1; x < gridNumber; x++)
		{
			index = x + (y*(gridNumber+2));
			if(grid[index])
			{
				if(!hasEdge)
				{
					front = x;
					hasEdge = true;
				}
				else
				{
					back = x;
					for(ite = front+1; ite < back; ite++)
					{
						index = ite + (y*(gridNumber+2));
						grid[index] = true;
					}
					front = x;
				}
			}
		}
	}
	return result;
}

double cross(Stroke::Point &o, Stroke::Point &a, Stroke::Point &b)
{
	return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

bool compare(Stroke::Point &a, Stroke::Point &b)
{
	return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

Stroke::V_POINT ConvexHull::process(size_t verticesCount, Ogre::Vector3 *vertices, float *grid, size_t gridNumber)
{
	int i, x, y, t;
	unsigned int index;
	Stroke::Point p1;
	Stroke::Point p2;
	Stroke::Point dist;
	Stroke::V_POINT vPoint;
	Stroke::V_POINT vResult;
	for(i = 0;i < verticesCount;i++)
	{
		if(vertices[i].y < 1.0 && vertices[i].y > -1.0)//the y position at surface
		{
			p1.x = ((int)vertices[i].x);

			p1.y = ((int)vertices[i].z);
			
			//vPointSet.insert(p1);
			vPoint.push_back(p1);
		}
	}

	//for(ite = vPointSet.begin(); ite != vPointSet.end();++ite)
	//{
	//	vPoint.push_back(*ite);
	//}
	// 將所有點依照座標大小排序
	std::sort(vPoint.begin(), vPoint.end(), compare);

	int m = 0;  // m 為凸包頂點數目

	size_t size = vPoint.size();
	vResult.resize(size * 2);
	// 包下半部
	for (i=0; i<size; ++i)
	{
		while (m >= 2 && cross(vResult[m-2], vResult[m-1], vPoint[i]) <= 0) m--;
		vResult[m++] = vPoint[i];
	}

	// 包上半部，不用再包入方才包過的終點，但會再包一次起點
	for (i=size-2, t=m+1; i>=0; --i)
	{
		while (m >= t && cross(vResult[m-2], vResult[m-1], vPoint[i]) <= 0) m--;
		vResult[m++] = vPoint[i];
	}

	m--;    // 最後一個點是重複出現兩次的起點，故要減一。
	vResult.resize(m);
	for(i = 0; i < m; i++)
	{
		p1 = vResult[i];
		p2 = vResult[(i+1)%m];
		x = (int)p1.x;
		y = (int)p1.y;
		index = x +  (y*(gridNumber+2));
		grid[index] = 1.0;
		while(p2.distance(p1) > 1.0)
		{
			dist = (p2 - p1).normalisedCopy();
			p1 += dist;
			x = (int)p1.x;
			y = (int)p1.y;
			index = x +  (y*(gridNumber+2));
			grid[index] = 1.0;
		}
		x = (int)p2.x;
		y = (int)p2.y;
		index = x +  (y*(gridNumber+2));
		grid[index] = 1.0;
	}
	return vResult;
}


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

	// �N�Ҧ��I�̷Ӯy�Фj�p�Ƨ�
	std::sort(vPoint.begin(), vPoint.end());

	int m = 0;  // m ���Y�]���I�ƥ�

	size_t size = vPoint.size();
	vResult.resize(size * 2);
	// �]�U�b��
	for (i=0; i<size; ++i)
	{
		while (m >= 2 && CROSS(vResult[m-2], vResult[m-1], vPoint[i]) <= 0) m--;
		vResult[m++] = vPoint[i];
	}

	// �]�W�b���A���ΦA�]�J��~�]�L�����I�A���|�A�]�@���_�I
	for (i=size-2, t=m+1; i>=0; --i)
	{
		while (m >= t && CROSS(vResult[m-2], vResult[m-1], vPoint[i]) <= 0) m--;
		vResult[m++] = vPoint[i];
	}

	m--;    // �̫�@���I�O���ƥX�{�⦸���_�I�A�G�n��@�C
	vResult.resize(m);

	return vResult;
}
#ifndef _PARTICLE_DRAW_H_
#define _PARTICLE_DRAW_H_

#include "stroke_draw.h"
#include <ogre.h>

namespace Stroke
{
	class ParticleDraw : StrokeDraw
	{
	private:
		Ogre::ParticleSystem *					m_pPS;
		Ogre::SceneNode *						m_pPSNode;

	public:
		ParticleDraw(void);
		virtual ~ParticleDraw(void);
		virtual bool isValid(void);
		virtual void init(void);
		virtual void release(void);
		virtual void drawBegin(void);
		virtual void draw(const LIST_POINT &list);
		virtual void drawEnd(void);
		void setAttribute(const std::string &materail, unsigned int size);
	};
}

#endif //_OGRE_PARTICLE_DRAW_H_
#include "particle_draw.h"

#define PROJECT_NAME "ChinesePaint"
#define PARTICLE_MOVE_SPEED 100
#define PARTICLE_LIVE_TIME 10
#define PARTICLE_SIZE_X 0.5
#define PARTICLE_SIZE_Y 0.5

namespace Stroke
{
	ParticleDraw::ParticleDraw(void):m_pPS(NULL), 
		m_pPSNode(NULL)
	{
	}

	ParticleDraw::~ParticleDraw(void)
	{
		release();
	}

	bool ParticleDraw::isValid(void)
	{
		return m_pPS != NULL && m_pPSNode != NULL;
	}

	void ParticleDraw::init(void)
	{
		release();

		Ogre::SceneManager *mgr = Ogre::Root::getSingleton().getSingleton().getSceneManager(PROJECT_NAME);
		if(mgr != NULL)
		{
			m_pPSNode = mgr->getRootSceneNode()->createChildSceneNode();

			m_pPS = mgr->createParticleSystem();

			m_pPSNode->attachObject(m_pPS);
		}
	}

	void ParticleDraw::release(void)
	{
		if(m_pPS != NULL)
		{
			Ogre::SceneManager *mgr = Ogre::Root::getSingleton().getSingleton().getSceneManager(PROJECT_NAME);
			if(mgr != NULL)
			{
				m_pPS->clear();
				m_pPSNode->detachObject(m_pPS);
				mgr->destroySceneNode(m_pPSNode);
				m_pPSNode = NULL;
				mgr->destroyParticleSystem(m_pPS);
				m_pPS = NULL;
			}
		}
	}

	void ParticleDraw::drawBegin(void)
	{
	}

	void ParticleDraw::draw(const V_POINT &vect, float *density)
	{
		V_POINT::const_iterator ite;
		Ogre::Particle* particle = NULL;
		Ogre::ParticleIterator p_ite = m_pPS->_getIterator();
		for(ite = vect.begin(); ite != vect.end(); ++ite)
		{
			if(!p_ite.end())
			{
				particle = p_ite.getNext();
				particle->position.x = ite->x;
				particle->position.y = 0.0;
				particle->position.z = ite->y;
			}
			else
			{
				particle = m_pPS->createParticle();
				if(particle != NULL)
				{
					particle->position.x = ite->x;
					particle->position.y = 0;
					particle->position.z = ite->y;
					particle->timeToLive = PARTICLE_LIVE_TIME;
					particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
				}
			}
		}
	}

	void ParticleDraw::draw(const LIST_POINT &list, float *density)
	{
		LIST_POINT::const_iterator ite;
		Ogre::Particle* particle = NULL;
		Ogre::ParticleIterator p_ite = m_pPS->_getIterator();
		for(ite = list.begin(); ite != list.end(); ++ite)
		{
			if(!p_ite.end())
			{
				particle = p_ite.getNext();
				particle->position.x = ite->x;
				particle->position.y = 0.0;
				particle->position.z = ite->y;
			}
			else
			{
				particle = m_pPS->createParticle();
				if(particle != NULL)
				{
					particle->position.x = ite->x;
					particle->position.y = 0;
					particle->position.z = ite->y;
					particle->timeToLive = PARTICLE_LIVE_TIME;
					particle->setDimensions (PARTICLE_SIZE_X, PARTICLE_SIZE_Y);
				}
			}
		}
	}

	void ParticleDraw::drawEnd(void)
	{
	}

	void ParticleDraw::setAttribute(const std::string &materail, unsigned int size)
	{
		m_pPS->setMaterialName(materail);
		m_pPS->setParticleQuota(size);
	}
}
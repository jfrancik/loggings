#pragma once

#include "set"

class CGameObject;

typedef std::list<CGameObject*> CCGameObjectList;

class CGameObject : public CSprite
{
public:
	CGameObject(float x, float y, float w, float h, Uint32 time) : CSprite(x + w / 2, y + h / 2, w, h, time) { }
	CGameObject(float x, float y, float w, float h, char *pFileBitmap, Uint32 time) : CSprite(x + w / 2, y + h / 2, pFileBitmap, time) { SetSize(w, h); }

	int is(string tag);
	void Set(string tag);
	void Set(string tag, int val);
};

class CLogging : public CGameObject
{
	bool m_bWalking, m_bRight, m_bBlocked, m_bGhostling;
public:
	CLogging(float x, float y, bool bRight, long time);
	void Walk(bool bRight, CGameObject *pPlatform = NULL);
	void Walk(CGameObject *pPlatform = NULL);
	void Bounce(CGameObject *pWall = NULL);
	void Stand(bool bRight);
	void Stand();
	void Block();
	void Fall();
	void Drop(CGameObject *pDropper);
	void Sink(CGameObject *pSink);
	void Switch(CGameObject *pSwitch);
	void Jump();
	void Ghostling();
	void Explode();
};

class CPlatform : public CGameObject
{
public:
	CPlatform(float x, float y, float l, long time);
};

class CDropper : public CGameObject
{
public:
	CDropper(float x, float y, float l, long time);
};

class CSink : public CGameObject
{
	unsigned m_n;
	CSprite m_effect;
	set<CLogging*> m_loggings;
public:
	CSink(float x, float y, unsigned n, long time);
	void Sink(CLogging *pLogging);
	virtual void OnUpdate(Uint32 time, Uint32 deltaTime);
	virtual void OnPrepareGraphics(CGraphics *g);
};

class CWall : public CGameObject
{
public:
	CWall(float x, float y, long time);
	CWall(float x, float y, bool penetrable, long time);
	void SwitchPenetrable();
};

class CSwitch : public CGameObject
{
	CCGameObjectList &m_sprites;
public:
	CSwitch(float x, float y, CCGameObjectList &sprites, long time);
	void Switch();
};
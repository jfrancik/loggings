#include "stdafx.h"
#include "MyObjects.h"


const float H_PLATFORM = 20;
const float H_WALL = 120;
const float W_STICK = 20;
const float H_STICK = 40;
const float W_SINK = 60;
const float W_SWITCH = 20;
const float H_SWITCH = 20;
const float V = 120;

int CGameObject::is(string tag)
{
	string my_tag = GetProperty("tag");
	if (my_tag == tag) return 1;
	int prop = GetProperty((char*)tag.c_str());
	return prop;
}

void CGameObject::Set(string tag)
{
	SetProperty("tag", (char*)tag.c_str());
}

void CGameObject::Set(string tag, int val)
{
	SetProperty((char*)tag.c_str(), val);
}

CLogging::CLogging(float x, float y, bool bRight, long time)
	: CGameObject(x, y, W_STICK, H_STICK, time)
{
	LoadImage("stick40.bmp", "fall", 7, 2, 0, 1, CColor::White());
	LoadImage("stick40.bmp", "stand_right", 7, 2, 1, 1, CColor::White());
	LoadImage("stick40.bmp", "stand_left", 7, 2, 2, 1, CColor::White());
	AddImage("stick40.bmp", "walk_right", 7, 2, 3, 1, 4, 1, CColor::White());
	AddImage("stick40.bmp", "walk_left", 7, 2, 5, 1, 6, 1, CColor::White());
	AddImage("stick40.bmp", "death", 7, 2, 0, 0, 6, 0, CColor::White());
	AddImage("explosion.png", "explode", 1, 48, 0, 47, 0, 0, CColor::Black());
	LoadImage("ghostling.png", "ghost", CColor::White());

	Set("logging");
	m_bWalking = false;
	m_bRight = bRight;
	m_bBlocked = false;
	m_bGhostling = false;
	Stand();
}

void CLogging::Walk(bool bRight, CGameObject *pPlatform)
{
	if (IsDying()) return;
	m_bWalking = true;
	m_bRight = bRight;
	if (!m_bGhostling) SetAnimation(bRight ? "walk_right" : "walk_left", 0, 2, 100, 0);
	SetVelocity(bRight ? V : -V, 0);
	if (pPlatform != NULL)
		SetBottom(pPlatform->GetTop());
}

void CLogging::Walk(CGameObject *pPlatform)
{
	Walk(m_bRight, pPlatform);
}

void CLogging::Bounce(CGameObject *pWall)
{
	if (!m_bWalking || m_bBlocked) return;
	if (pWall != NULL && Dot(pWall->GetPosition() - GetPosition(), GetVelocity()) < 0) return;
	Walk(!m_bRight);
}

void CLogging::Stand(bool bRight)
{
	m_bRight = bRight;
	Stand();
}

void CLogging::Stand()
{
	m_bWalking = false;
	m_bGhostling = false;
	SetImage(m_bRight ? "stand_right" : "stand_left");
	SetVelocity(0, GetVelocity().Y());
}

void CLogging::Block()
{
	m_bBlocked = true;
	m_bGhostling = false;
	Set("blocked", 1);
	SetImage(m_bRight ? "stand_right" : "stand_left");
	SetVelocity(0, 0);
}

void CLogging::Fall()
{
	if (IsDying()) return;
	m_bWalking = false;
	if (!m_bGhostling) SetImage("fall");
	Accelerate(0, -10);
}

void CLogging::Drop(CGameObject *pDropper)
{
	m_bWalking = false;
	if (!m_bGhostling) SetImage("fall");
	SetX(pDropper->GetX());
	Accelerate(0, -10);
	SetVelocity(0, GetVelocity().Y());
}

void CLogging::Sink(CGameObject *pSink)
{
	if (m_bRight && GetLeft() < pSink->GetLeft() + 20)
		return;
	if (!m_bRight && GetRight() > pSink->GetRight() - 20)
		return;

	m_bWalking = false;
	Accelerate(0, -1);
	SetVelocity(0, GetVelocity().Y());
	if (!IsDying())
	{
		SetAnimation("death", 4);
		Die(1500);
	}
	if (pSink)
		((CSink*)pSink)->Sink(this);	// potentially dangerous cast!
}

void CLogging::Switch(CGameObject *pSwitch)
{
	if (pSwitch)
		((CSwitch*)pSwitch)->Switch();	// potentially dangerous cast!
}

void CLogging::Jump()
{
	if (!IsDying())
	{
		Move(0, 2);
		SetVelocity(GetVelocity().X(), 250);
	}
}

void CLogging::Ghostling()
{
	m_bGhostling = true;
	Set("ghostling", 1);
	SetImage("ghost");
}

void CLogging::Explode()
{
	if (!IsDying())
	{
		SetVelocity(0, 0);
		SetAnimation("explode", 16);
		Die(2000);
	}
}

CPlatform::CPlatform(float x, float y, float l, long time)
	: CGameObject(x, y, l, H_PLATFORM, "black.png", time)
{
	Set("platform");
}

CDropper::CDropper(float x, float y, float l, long time)
	: CGameObject(x, y, l, H_PLATFORM, "red.png", time)
{
	Set("dropper");
}

CSink::CSink(float x, float y, unsigned n, long time)
	: CGameObject(x, y, W_SINK, H_PLATFORM, "sink.png", time),
	  m_effect(x + W_SINK / 2, y + H_PLATFORM / 2 + 30, 0, 0, time)
{
	Set("sink");
	m_effect.AddImage("animation002.png", "magic", 9, 1, 0, 0, 8, 0, CColor::Black());
	m_effect.SetAnimation("magic", 32);
	m_n = n;
}

void CSink::Sink(CLogging *pLogging)
{
	m_loggings.insert(pLogging);
	if (m_loggings.size() >= m_n)
		Die(100);
}

void CSink::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	CGameObject::OnUpdate(time, deltaTime);
	m_effect.Update(time);
}

void CSink::OnPrepareGraphics(CGraphics *g)
{
	m_effect.Draw(g);
	CGameObject::OnPrepareGraphics(g);
	if (!m_effect.IsDead() && m_loggings.size() <= m_n)
		*g << xy((int)GetX(), (int)GetY() + 40) << left << m_n - m_loggings.size() << flush;
}


CWall::CWall(float x, float y, long time)
	: CGameObject(x, y, H_PLATFORM, H_WALL, time)
{
	LoadImage("wall.png", "solid", 6, 1, 0, 0, CColor::White());
	LoadImage("wall.png", "image_inpenetrable", 6, 1, 1, 0, CColor::White());
	AddImage("wall.png", "image_penetrable", 6, 1, 2, 0, 5, 0, CColor::White());

	SetImage("solid");
	Set("wall");
}

CWall::CWall(float x, float y, bool penetrable, long time)
	: CGameObject(x, y, H_PLATFORM, H_WALL, time)
{
	LoadImage("wall.png", "solid", 6, 1, 0, 0, CColor::White());
	LoadImage("wall.png", "image_inpenetrable", 6, 1, 1, 0, CColor::White());
	AddImage("wall.png", "image_penetrable", 6, 1, 2, 0, 5, 0, CColor::White());

	if (penetrable)
		SetAnimation("image_penetrable");
	else
		SetImage("image_inpenetrable");

	Set("penetrable", penetrable ? 1 : 0);
	Set("enhanced_wall", 1);
	Set("wall");
}

void CWall::SwitchPenetrable()
{
	if (is("penetrable"))
	{
		SetImage("image_inpenetrable");
		Set("penetrable", 0);
	}
	else
	{
		SetAnimation("image_penetrable");
		Set("penetrable", 1);
	}
}

CSwitch::CSwitch(float x, float y, CCGameObjectList &sprites, long time)
	: CGameObject(x, y, W_SWITCH, H_SWITCH, time),
	m_sprites(sprites)
{
	LoadImage("switch.png", "off", 2, 1, 0, 0, CColor::White());
	LoadImage("switch.png", "on", 2, 1, 1, 0, CColor::White());
	SetImage("off");
	//Set("on", 0);
	Set("switch");
}

void CSwitch::Switch()
{
	if (is("on"))
		return;

	SetImage("on");
	Set("on");
	for (CGameObject *pSprite : m_sprites)
		if (pSprite->is("enhanced_wall"))
			((CWall*)pSprite)->SwitchPenetrable();
}
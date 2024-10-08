#include "stdafx.h"
#include "MyObjects.h"
#include "MyGame.h"

CMyGame::CMyGame(void) :
	m_block(50, 50, "block.png", 0),
	m_jump(130, 50, "jump.png", 0),
	m_ghost(130, 50, "ghost.png", 0),
	m_explode(210, 50, "explode.png", 0)
{
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();
	m_bLevelCompleted = true;
	m_bLevelActive = false;

	for (CGameObject *pSprite : m_sprites)
	{
		if (pSprite->is("logging"))
		{
			CLogging *pLogging = static_cast<CLogging*>(pSprite);

			if (!pSprite->is("blocked"))
				pLogging->Fall();

			for (CGameObject *pObstacle : m_sprites)
			{
				if (pObstacle == pLogging) continue;
				if (pObstacle->IsDead()) continue;
				if (!pLogging->HitTest(pObstacle, 0)) continue;
				
				if (pObstacle->is("dropper"))
					pLogging->Drop(pObstacle);

				if (pObstacle->is("sink"))
					pLogging->Sink(pObstacle);

				if (pObstacle->is("switch"))
					pLogging->Switch(pObstacle);

				// HERE IS YOUR CODE TO COMPLETE
				// pLogging is your logging
				// pObstacle is the obstacle he encountered - platform, wall, or another logging
				//




			}

			if (pLogging->GetTop() < 0) pLogging->Die(0);
		}

		// Completion tests
		if (pSprite->is("logging") && !pSprite->IsDead() && !pSprite->is("blocked"))
			m_bLevelActive = true;					// any alive and moving ligging means the game active
		if (pSprite->is("sink") && !pSprite->IsDead())	// any alive sink means game not completed
			m_bLevelCompleted = false;

		pSprite->Update(t);
	}
}

void CMyGame::OnDraw(CGraphics* g)
{
	for (CGameObject *pSprite : m_sprites)
		pSprite->Draw(g);
	m_block.Draw(g);
	//m_jump.Draw(g);
	m_ghost.Draw(g);
	m_explode.Draw(g);

	*g << top << right << "LEVEL " << GetLevel();
	if (m_bGameCompleted)
		*g << center << vcenter << up << up << "CONGRATULATIONS!!!" << endl << "You have completed all levels!" << flush;
	else if (m_bLevelCompleted)
		*g << endl << endl << center << "LEVEL COMPLETED" << endl << "Press F2 to repeat the level or any other key to proceed" << flush;
	else if (!m_bLevelActive)
		*g << endl << endl << center << "LEVEL FAILED" << endl << "Press F2 to repeat the level" << flush;
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	m_block.LoadImage("block on.png", "on");
	m_jump.LoadImage("jump on.png", "on");
	m_ghost.LoadImage("ghost on.png", "on");
	m_explode.LoadImage("explode on.png", "on");
	m_block.LoadImage("block.png", "off");
	m_jump.LoadImage("jump.png", "off");
	m_ghost.LoadImage("ghost.png", "off");
	m_explode.LoadImage("explode.png", "off");
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	// Clean up first
	for (CGameObject* pSprite : m_sprites)
		delete pSprite;
	m_sprites.clear();

	// UI
	m_mode = NONE;
	m_block.SetImage("off");
	m_jump.SetImage("off");
	m_ghost.SetImage("off");
	m_explode.SetImage("off");

	m_bGameCompleted = false;

	switch (nLevel)
	{
	case 1:// build Level 1 sprites

		// platforms
		m_sprites.push_back(new CPlatform(400, 600, 3000, GetTime()));
		m_sprites.push_back(new CDropper(350, 600, 50, GetTime()));

		m_sprites.push_back(new CPlatform(200, 400, 500, GetTime()));
		m_sprites.push_back(new CWall(200, 400, GetTime()));

		m_sprites.push_back(new CPlatform(250, 250, 700, GetTime()));

		m_sprites.push_back(new CPlatform(50, 100, 400, GetTime()));
		m_sprites.push_back(new CSink(370, 100, 5, GetTime()));

		// loggings
		for (int i = 0; i < 10; i++)
			m_sprites.push_back(new CLogging(600 + (float)i * 200, 700, false, GetTime()));

		break;

	case 2:// Level 2 (EMPTY)

		m_sprites.push_back(new CPlatform(-1800, 600, 2100, GetTime()));
		m_sprites.push_back(new CDropper(300, 600, 50, GetTime()));

		m_sprites.push_back(new CPlatform(300, 400, 600, GetTime()));
		m_sprites.push_back(new CWall(600, 400, GetTime()));
		m_sprites.push_back(new CSwitch(750, 420, m_sprites, GetTime()));

		m_sprites.push_back(new CPlatform(100, 250, 600, GetTime()));
		m_sprites.push_back(new CWall(300, 250, 0, GetTime()));

		m_sprites.push_back(new CPlatform(250, 100, 600, GetTime()));
		m_sprites.push_back(new CSink(470, 100, 5, GetTime()));

		// loggings
		for (int i = 0; i < 10; i++)
			m_sprites.push_back(new CLogging(-1800 + (float)i * 200, 700, true, GetTime()));

		break;

	case 3:
		m_bGameCompleted = true;
		break;

	default:
		SetLevel(1);
	}
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	//if (sym == SDLK_SPACE)
		//PauseGame();

	if (sym == SDLK_F2)
		OnStartLevel(GetLevel());
	//else if (sym == SDLK_F3)
	//	NewLevel();
	else if (m_bLevelCompleted)
		NewLevel();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
	if (m_block.HitTest(x, y))
	{
		m_mode = BLOCK;
		m_block.SetImage("on");
		m_jump.SetImage("off");
		m_ghost.SetImage("off");
		m_explode.SetImage("off");
	}

	if (m_jump.HitTest(x, y))
	{
		m_mode = JUMP;
		m_block.SetImage("off");
		m_jump.SetImage("on");
		m_ghost.SetImage("off");
		m_explode.SetImage("off");
	}

	if (m_ghost.HitTest(x, y))
	{
		m_mode = GHOST;
		m_block.SetImage("off");
		m_jump.SetImage("off");
		m_ghost.SetImage("on");
		m_explode.SetImage("off");
	}

	if (m_explode.HitTest(x, y))
	{
		m_mode = EXPLODE;
		m_block.SetImage("off");
		m_jump.SetImage("off");
		m_ghost.SetImage("off");
		m_explode.SetImage("on");
	}


	for (CGameObject* pSprite : m_sprites)
		if (pSprite->is("logging") && pSprite->HitTest(x, y, 5) && !pSprite->IsDying())
		{
			CLogging* pLogging = (CLogging*)pSprite;
			switch (m_mode)
			{
			case BLOCK: pLogging->Block(); break;
			case JUMP: pLogging->Jump(); break;
			case GHOST: pLogging->Ghostling(); break;
			case EXPLODE: pLogging->Explode(); break;
			}
			break;
		}
}

void CMyGame::OnLButtonUp(Uint16 x, Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x, Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x, Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x, Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x, Uint16 y)
{
}

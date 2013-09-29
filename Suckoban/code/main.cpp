#include "GineUtils.h"
#include "Input.h"
#include "PostProcess.h"
#include "Gameplay.h"
#include "StateMachine.h"

// Main game properties
const char* GAME_TITLE      = "Suckoban";
const bool  ENABLE_4XMSAA   = false;
const bool  ENABLE_SSAO     = false;
const int   PANIC_CLOSE_KEY = VK_F4;

/// <summary>
/// Main Game class
/// Used to init DX11 and Gine
/// </summary>

class Game : public D3DApp
{
public:
  Game(HINSTANCE hInstance);
  ~Game();

  bool Init();
  void OnResize();
  void UpdateScene(float dt);
  void DrawScene();
};

/// <summary>
/// Option's windows before starting the game
/// </summary>

void ShowPregameOptions(Game* game)
{
#if defined(DEBUG) | defined(_DEBUG)

  Info::ShowConsole();

#else

  int result = MessageBox(0, L"Start in fullscreen mode?", Utils::ToWString(GAME_TITLE).c_str(),
                          MB_YESNO | MB_ICONQUESTION);
  if(IDYES == result)
    game->SetFullscreen(true);

#endif
}

/// <summary>
/// Application entry point
/// </summary>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
  srand((UINT)time(0));
  Game game(hInstance);

  ShowPregameOptions(&game);

  if(!game.Init()) {
    Info::Fatal("Application init failed");
    return 0;
  }

  Info::Log("Run...\n");
  return game.Run();
}

Game::Game(HINSTANCE hInstance) :
  D3DApp(hInstance)
{
  mMainWndCaption = Utils::ToWString(GAME_TITLE);
  mEnable4xMsaa = ENABLE_4XMSAA;
  mEnableSSAO = ENABLE_SSAO;
}

Game::~Game()
{
  gContext->ClearState();
  Gine::Destroy();
}

bool Game::Init()
{
  if(!D3DApp::Init())
    return false;
  if(!Gine::Init())
    return false;

  Gameplay::GetInstance()->Init();
  StateMachine::Push(Gameplay::GetInstance());

  return true;
}

void Game::OnResize()
{
  D3DApp::OnResize();
}

void Game::UpdateScene(float dt)
{
  if(Input::Pressed(PANIC_CLOSE_KEY))
    PostQuitMessage(0);

  Input::Tick(dt);

  StateMachine::Tick(dt);

  gCamera->UpdateViewMatrix();
}

void Game::DrawScene()
{  
  StateMachine::Draw();
  HR(mSwapChain->Present(0, 0));
}
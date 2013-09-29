#include "Gameplay.h"
#include "Effects.h"
#include "Sprite.h"
#include "Input.h"
#include "PostProcess.h"
#include "StateMachine.h"

Gameplay Gameplay::mGameplay;

Gameplay::Gameplay() :
  State(),
  miLevel(0),
  mLevel(0)
{
}

bool Gameplay::Init()
{
  if(mInitialized)
    Info::Fatal("Gameplay already initialized");

  State::Init();

  if(!Level::Load("data/maps/maps.txt", mLevels))
    return false;

  miLevel = 0;
  mLevel = &mLevels[miLevel];
  mLevel->Reset();

  mCamera.SetLens(0.25f * MathHelper::Pi, float(gScreenW) / gScreenH, 0.1f, 100.0f);
  Gine::gCamera = &mCamera;

  DirectionalLight light;
  light.Ambient   = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
  light.Diffuse   = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
  light.Specular  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
  light.Direction = XMFLOAT3(0.5f, -1.0f, 0.5f);
  Effects::BasicFX->SetDirLight(&light);

  // Set UI
  float screenMargin = 20.0f;
  float iconH = 40.0f;
  float fontX = 60.0f;

  mUIFont.Load("Lucida Console", 30, XMCOLOR(0xffeeeeec), FONTSTYLE_REGUAR);
  mUISteps.Set("", XMFLOAT2(fontX, screenMargin), TEXTALIGN_LEFT, &mUIFont);
  mUIPushes.Set("", XMFLOAT2(fontX, iconH + screenMargin), TEXTALIGN_LEFT, &mUIFont);
  mUITime.Set("", XMFLOAT2(Gine::gScreenW - 60.0f, 20.0f), TEXTALIGN_RIGHT, &mUIFont);

  if(!LevelPause::GetInstance()->Init())
    return false;

  mInitialized = true;

  return true;
}

void Gameplay::Pause()
{
  State::Pause();
  mPostProcess.SetPostProcessFilter(POST_PROCESS_FILTER_BLUR, NONE, 0.3f);
}

void Gameplay::Resume()
{
  State::Resume();
  mPostProcess.ClearPostProcessFilter(NONE, 0.3f);
}

void Gameplay::UpdateCamera(float dt)
{
  static float pX = 0.0f;
  static float pY = 0.0f;
  static float pZ = 0.0f;
  const float speed = 5.0f;

  if(Input::Down(VK_NUMPAD8)) pY += speed * dt;
  if(Input::Down(VK_NUMPAD2)) pY -= speed * dt;
  if(Input::Down(VK_NUMPAD4)) pX -= speed * dt;
  if(Input::Down(VK_NUMPAD6)) pX += speed * dt;
  if(Input::Down(VK_NUMPAD7)) pZ += speed * dt;
  if(Input::Down(VK_NUMPAD9)) pZ -= speed * dt;

  const XMFLOAT3& pos    = mLevel->GetPlayerPosition() + XMFLOAT3(0.0f, 10.0f, -5.0f) + XMFLOAT3(pX, pY, pZ);
  const XMFLOAT3& target = mLevel->GetPlayerPosition();
  const XMFLOAT3& up     = XMFLOAT3(0.0f, 1.0f, 0.0f);
  mCamera.LookAt(pos, target, up);
}

void Gameplay::UpdateUI(float dt)
{
  mUISteps.Value = Gine::Utils::ToString(mLevel->GetSteps());
  mUIPushes.Value = Gine::Utils::ToString(mLevel->GetPushes());

  int seconds = (int)mLevel->GetTime();
  int minutes = seconds / 60;
  int onlySeconds = seconds - minutes * 60;
  mUITime.Value = Gine::Utils::Format("%d:%02d", minutes, onlySeconds);
}

void Gameplay::Tick(float dt)
{
  State::Tick(dt);

  UpdateCamera(dt);
  UpdateUI(dt);

  if(mPaused)
    return;

  mLevel->Tick(dt);
  if(mLevel->IsSolved())
  {
    mLevel = &mLevels[++miLevel];
    mLevel->Reset();
    return;
  }

  if(Input::Released('R'))
  {
    mLevel->Reset();
    return;
  }

  if(Input::Released('P'))
  {
    if(!LevelPause::GetInstance()->Alive)
      StateMachine::Push(LevelPause::GetInstance());
  }
}

void Gameplay::DrawUI()
{
  int screenMargin = 20;
  int iconH = 40;

  Sprite::Draw("footsteps", screenMargin, screenMargin, 0.3f);
  Sprite::Draw("move", screenMargin, iconH + screenMargin, 0.3f);
  bool isUndo = mLevel->IsUndo();
  Sprite::Draw(isUndo ? "undo" : "undo_g", screenMargin, 2*iconH + screenMargin, isUndo ? 0.5f : 0.3f);
  Sprite::Draw("clock", Gine::gScreenW - 50, 20, 0.3f);

  mUISteps.Draw();
  mUIPushes.Draw();
  mUITime.Draw();
}

void Gameplay::Draw()
{
  // Update effects camera
	XMMATRIX view     = mCamera.View();
	XMMATRIX proj     = mCamera.Proj();
	XMMATRIX viewProj = mCamera.ViewProj();
  Effects::BasicFX->SetEyePosW(mCamera.GetPosition());
  Effects::BasicFX->SetViewProj(viewProj);

  // Level draw
  mLevel->Draw();
  
  // UI draw
  DrawUI();
}
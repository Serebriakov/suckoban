#include "Info.h"
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include "GineUtils.h"
using std::string;
using std::wstring;
using namespace Gine;

void Info::ShowConsole()
{
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  AllocConsole();

  HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
  int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
  FILE* hf_out = _fdopen(hCrt, "w");
  setvbuf(hf_out, NULL, _IONBF, 1);
  *stdout = *hf_out;

  HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
  hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
  FILE* hf_in = _fdopen(hCrt, "r");
  setvbuf(hf_in, NULL, _IONBF, 128);
  *stdin = *hf_in;
}

void Info::Log(const char* aText, ...)
{
  char buff[1024];
  va_list args;
  va_start(args, aText);
  vsprintf_s(buff, aText, args);
  va_end(args);

  printf("%s\n", buff);
}

void Info::Fatal(const char* aText, ...)
{
  char buff[1024];
  va_list args;
  va_start(args, aText);
  vsprintf_s(buff, aText, args);
  va_end(args);

  Log(buff);

  if(!DISABLE_MSG)
    MessageBox(0, Utils::ToWString(&string(buff)).c_str(), L"Fatal error", MB_OK | MB_ICONERROR);
}

void Info::Print(XMFLOAT4X4& aMatrix)
{
  Info::Log("%f\t%f\t%f\t%f", aMatrix._11, aMatrix._12, aMatrix._13, aMatrix._14);
  Info::Log("%f\t%f\t%f\t%f", aMatrix._21, aMatrix._22, aMatrix._23, aMatrix._24);
  Info::Log("%f\t%f\t%f\t%f", aMatrix._31, aMatrix._32, aMatrix._33, aMatrix._34);
  Info::Log("%f\t%f\t%f\t%f", aMatrix._41, aMatrix._42, aMatrix._43, aMatrix._44);
}
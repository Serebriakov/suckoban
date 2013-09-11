#include "GineUtils.h"
#include <vector>
#include <cstdarg>

wstring Gine::Utils::ToWString(const string* s)
{
  return wstring(s->begin(), s->end());
}

string Gine::Utils::ToString(const int i)
{
  return to_string((long double)i);
}

string Gine::Utils::Format(const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  std::string buf = Gine::Utils::VFormat(fmt, ap);
  va_end (ap);
  return buf;
}

string Gine::Utils::VFormat(const char *fmt, va_list ap)
{
  // Allocate a buffer on the stack that's big enough for us almost
  // all the time.  Be prepared to allocate dynamically if it doesn't fit.
  size_t size = 1024;
  char stackbuf[1024];
  vector<char> dynamicbuf;
  char *buf = &stackbuf[0];

  while (1) {
    // Try to vsnprintf into our buffer.
    int needed = vsnprintf (buf, size, fmt, ap);
    // NB. C99 (which modern Linux and OS X follow) says vsnprintf
    // failure returns the length it would have needed.  But older
    // glibc and current Windows return -1 for failure, i.e., not
    // telling us how much was needed.

    if (needed <= (int)size && needed >= 0) {
      // It fit fine so we're done.
      return string(buf, (size_t) needed);
    }

    // vsnprintf reported that it wanted to write more characters
    // than we allotted.  So try again using a dynamic buffer.  This
    // doesn't happen very often if we chose our initial size well.
    size = (needed > 0) ? (needed+1) : (size*2);
    dynamicbuf.resize (size);
    buf = &dynamicbuf[0];
  }
}
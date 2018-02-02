#include "Logger.h"
#include <stdarg.h>

Logger gLogger;

void Logger::init()
{
  fopen_s(&m_fs, LOG_FILENAME, "w+");
  m_bActive = true;
  print(LOG_LEVEL_INFO, "Log init, lvl %d\n", LOG_LEVEL);
}


void Logger::close()
{
  print(LOG_LEVEL_INFO, "Log complete\n");
  m_bActive = false;
  fclose(m_fs);
}


void Logger::print(LogLevel lvl, const char* s, ...)
{
  if (m_bActive && lvl >= LOG_LEVEL)
  {
    va_list argptr;
    va_start(argptr, s);
    vfprintf(m_fs, s, argptr);
    va_end(argptr);
    fflush(m_fs);
  }
}
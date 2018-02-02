#ifndef DX_GAME_LOGGING_H
#define DX_GAME_LOGGING_H

#include <fstream>
#include <sstream>
#include <string>

#define LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_FILENAME "Logs/log.txt"

typedef enum LogLevel_
{
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_NONE
} LogLevel;

#define LOGD(fstr, ...) gLogger.print(LOG_LEVEL_DEBUG, "<D:%s:%d> " fstr "\n", __FILE__, __LINE__, __VA_ARGS__)
#define LOGI(fstr, ...) gLogger.print(LOG_LEVEL_INFO, "<I:%s:%d> " fstr "\n", __FILE__, __LINE__, __VA_ARGS__)
#define LOGW(fstr, ...) gLogger.print(LOG_LEVEL_WARNING, "<W:%s:%d> " fstr "\n", __FILE__, __LINE__, __VA_ARGS__)
#define LOGE(fstr, ...) gLogger.print(LOG_LEVEL_ERROR, "<E:%s:%d> " fstr "\n", __FILE__, __LINE__, __VA_ARGS__)

class Logger
{
private:
  bool m_bActive;
  FILE * m_fs;

public:
  void init();
  void close();
  void print(LogLevel lvl, const char* s, ...);
};

extern Logger gLogger;

#endif

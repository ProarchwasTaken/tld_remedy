#include <raylib.h>
#include <plog/Init.h>
#include <plog/Severity.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/CsvFormatter.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Log.h>
#include "game.h"

using plog::ColorConsoleAppender, plog::RollingFileAppender, 
plog::CsvFormatter, plog::TxtFormatter;


void setupLogger() {
  static RollingFileAppender<CsvFormatter> file("log.csv", 1000000, 5);
  static ColorConsoleAppender<TxtFormatter> console;
  
  plog::Severity severity = plog::info;
  #ifndef NDEBUG
  severity = plog::debug;
  #endif // !NDEBUG

  plog::init(severity, &file).addAppender(&console);
  
  PLOGI << "Logger Initialized.";
  PLOGD << "Detected that the program is running on Dev Mode.";
}


int main(int argc, char *argv[]) {
  setupLogger();
  Game remedy;

  PLOGI << "Initializing the game...";
  remedy.init();

  PLOGI << "Starting the game loop.";
  remedy.start();
  
  return 0;
}


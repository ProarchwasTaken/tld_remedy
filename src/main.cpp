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
  static RollingFileAppender<CsvFormatter> file("log.csv", 100000, 3);
  static ColorConsoleAppender<TxtFormatter> console;
  plog::init(plog::debug, &file).addAppender(&console);
  
  PLOGI << "Logger Initialized.";
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


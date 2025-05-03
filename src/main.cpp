#include <raylib.h>
#include <plog/Init.h>
#include <plog/Severity.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/CsvFormatter.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Log.h>

using plog::ColorConsoleAppender, plog::RollingFileAppender, 
plog::CsvFormatter, plog::TxtFormatter;


void setupLogger() {
  static RollingFileAppender<CsvFormatter> file("log.csv", 8000, 3);
  static ColorConsoleAppender<TxtFormatter> console;
  plog::init(plog::debug, &file).addAppender(&console);
  
  PLOGI << "Logger Initialized.";
}


int main(int argc, char *argv[]) {
  setupLogger();
  InitWindow(1280, 720, "Project Remedy");
  SetTargetFPS(60);

  PLOGI << "Everything should be good to go!";
  while (!WindowShouldClose()) {
    BeginDrawing(); 
    {
      ClearBackground(WHITE);
    }
    EndDrawing();
  }
  
  CloseWindow();
  return 0;
}


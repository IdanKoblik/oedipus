#include <iostream>
#include <unistd.h>

#include "editor.h"

int main(int argc, char* argv[]) {
   try {
      if (argc < 2)
         throw std::runtime_error("No file provided");

      editor::Editor editor;
      editor.openFile(argv[1]);

      while (true) {
         editor.refreshScreen();

         char c;
         if (read(STDIN_FILENO, &c, 1) != 1)
            continue;

         if (editor.handleWriting(c) == 0)
            continue;

         if (editor.handleCursor(c) == 1)
            break;
      }
   } catch (const std::exception& e) {
      std::cerr << "Fatal: " << e.what() << std::endl;
      return 1;
   }

   return 0;
}

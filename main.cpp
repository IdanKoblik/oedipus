#include <unistd.h>
#include <termios.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace term {

inline std::ostream& endl(std::ostream& os) {
   return os << "\r\n";
}

}

class TerminalRawMode {
private:
   termios orig;
   
public:
   TerminalRawMode() {
      if (tcgetattr(STDIN_FILENO, &orig) == -1) {
          throw std::runtime_error(std::strerror(errno));
      }
      
      termios raw = orig;
      raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
      raw.c_oflag &= ~(OPOST);
      raw.c_cflag |= (CS8);
      raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
      raw.c_cc[VMIN]  = 0;
      raw.c_cc[VTIME] = 1;
      
      if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
          throw std::runtime_error(std::strerror(errno));
      }
   }

   ~TerminalRawMode() {
      tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
   }

   TerminalRawMode(const TerminalRawMode&) = delete;
   TerminalRawMode& operator=(const TerminalRawMode&) = delete;

};

#define CTRL_KEY(k) ((k) & 0x1f)

int main() {
   try {
      TerminalRawMode term;

      while (true) {
         char c;
         ssize_t n = read(STDIN_FILENO, &c, 1);
         if (n != 1)
            continue;

         if (c == CTRL_KEY('q'))
            break;
       
         std::cout << c << term::endl;
      }
   } catch (const std::exception &e) {
      std::cerr << "Fatal: " << e.what() << std::endl;
      return 1;
   }

   return 0;
}

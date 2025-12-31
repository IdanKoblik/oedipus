#ifndef RAW_H
#define RAW_H

#include <iostream>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define BACKSPACE 127

namespace terminal {

inline std::ostream& endl(std::ostream& os) {
   return os << "\r\n";
};

int enableRawMode(struct termios *term);
void disableRawMode(struct termios *term);

}

#endif // RAW_H

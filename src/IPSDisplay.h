#ifndef _IPSDISPLAY_H
#define _IPSDISPLAY_H

/* actual implementation unkown. just enough to get it compiling. */
#include <Arduino.h>

class IPSDisplay {
public:
    void show(String s) { Serial.println("<IPS Display emulation:> " + s);}
};

#endif 
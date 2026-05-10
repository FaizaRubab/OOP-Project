#include "Admin.h"

Admin::Admin() : Person() {}
Admin::Admin(int id, const char* name, const char* password):Person(id, name, "", password) 
{}
Admin::~Admin() {}

void Admin::displayMenu() {}
void Admin::saveToFile() {}

Admin Admin::fromFileLine(const char* line) 
{
    char buf[256];
    int i = 0;
    while (line[i] && i < 255) 
    {
        buf[i] = line[i]; 
        ++i; 
    }
    buf[i] = '\0';
    char* f[3]; 
    int fc = 0; 
    f[fc++] = buf;
    for (int j = 0; buf[j] && fc < 3; ++j)
    {
        if (buf[j] == ',') 
        {
            buf[j] = '\0'; 
            f[fc++] = &buf[j + 1]; 
        }
    }
    if (fc < 3)
    {
        return Admin();
    }

    int aid = 0; 
    for (int j = 0; f[0][j]; ++j)
    {
        aid = aid * 10 + (f[0][j] - '0');
    }
    return Admin(aid, f[1], f[2]);
}
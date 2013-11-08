#pragma once
#include <fstream>
#include <ostream>
#include <istream>
#include <string>
#include <vector>

#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define byte	unsigned char
#define uint	unsigned int
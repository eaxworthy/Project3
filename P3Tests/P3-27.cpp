// Autogenerated Scheme to C++ Code
// File: P3-27.cpp
#include <iostream>
#include "Object.h"
using namespace std;


int main ()
{
	Object __RetVal;
;
	cout << listop ("append", listop ("cdr", Object("(1 2 3 )") ), listop ("cdr", Object("(4 5 6 )") ));
	cout << endl;
	cout << listop ("cons", listop ("car", Object("(1 2 3 )") ), listop ("cdr", Object("(4 5 6 )") ));
	cout << endl;
	return 0;
}

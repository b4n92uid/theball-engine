#include <iostream>
#include "../sources/Any.h"

using namespace std;
using namespace tbe;

int main()
{
    Any var;

    var = 5;
    cout << var.GetValue<int>() << endl;

    var = true;
    cout << var.GetValue<bool>() << endl;

    var = 5;
    cout << var.GetValue<unsigned>() << endl;

    var = 5.9f;
    cout << var.GetValue<float>() << endl;

    var = 5.9;
    cout << var.GetValue<double>() << endl;

    var = string("C++ String");
    cout << var.GetValue<string > () << endl;

    var = new string("C++ Ptr String");
    cout << *var.GetValue<string*>() << endl;
	
    var = new char('A');
    cout << *var.GetValue<char*>() << endl;
	
	return 0;
}

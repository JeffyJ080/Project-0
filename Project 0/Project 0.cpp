#include <iostream>
#include <string>
using namespace std;

static string EnterNumber()
{
    string Input;
    cout << "Enter a number: ";
    getline(cin, Input);
    return("You entered: " + Input);
}

int main()
{
    cout << "Hello Jeffy\n";
    for (int i = 0; i < 10; i++)
    {
		cout << EnterNumber() << endl;
    }
}

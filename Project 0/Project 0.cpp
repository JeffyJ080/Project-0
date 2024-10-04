#include <iostream>
using namespace std;

static int EnterNumber(int iNumber)
{
    cout << "You entered: " << iNumber << endl;
}

int main()
{
    cout << "Hello Jeffy\n";
    for (int i = 0; i < 10; i++)
    {
        cout << "Enter a number: ";
        cin >> i;
		EnterNumber(i);
    }

}
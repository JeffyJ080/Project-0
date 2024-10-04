#include <iostream>
using namespace std;

int main()
{
    cout << "Hello Jeffy\n";
    for (int i = 0; i < 10; i++)
    {
		cout << "Enter a number: ";
        cin >> i;

		cout << "You entered: " << i << endl;
    }

}
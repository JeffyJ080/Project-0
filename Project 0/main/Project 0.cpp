#include <iostream>
#include <string>

#include "gui.h"
#include <thread>

using namespace std;
using namespace gui;

int __stdcall wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR arguments, int commandShow)
{
    CreateHWindow("Project 0", "Class 1");
    CreateDevice();
    CreateImGui();

    while (gui::exit)
    {
        BeginRender();
        Render();
        EndRender();

        this_thread::sleep_for(chrono::milliseconds(10));
    }

    // destroy gui
    DestroyImGui();
    DestroyDevice();
    DestroyHWindow();

    return EXIT_SUCCESS;
}

static string EnterNumber()
{
    string Input;
    cout << "Enter a number: ";
    getline(cin, Input);
    return("You entered: " + Input);
}

//int main()
//{
//    cout << "Hello Jeffy" << endl;
//    for (int i = 0; i < 10; i++)
//    {
//		cout << EnterNumber() << endl;
//    }
//}
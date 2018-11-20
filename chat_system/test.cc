#include <iostream>
#include <unistd.h>
#include "ChatWindow.hpp"

int main()
{
    ChatWindow *clip = new ChatWindow();
    clip->DrawList();
	clip->DrawHeader();
	clip->DrawOutput();
	clip->DrawInput();
    sleep(10);
    delete clip;
    return 0;
}

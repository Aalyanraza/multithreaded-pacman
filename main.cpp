#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <time.h>
#include "menu.h"
#include "shared_variables.h"
using namespace std;

int main() 
{
    pthread_t menu;
    pthread_create(&menu, NULL, menu_thread, NULL);
    sleep(1);
    while (gamerunning) {}
    sleep (1);
    return 0;
}

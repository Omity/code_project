/*
 * watchdogMain.cpp
 * 
 * Copyright 2021 rigol <SJ@ubuntu>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>
#include <thread>
#include <chrono>
#include "MultiThreadWatchdog.h"
 
int main() 
{
    // Initialize with 3sec expire time and max 10 threads.
    MultiThreadWatchdog wdg(3, 10);
 
    // Kick 5 times and stop.
    int i = 5;
    while(true) {
        if(i > 0) {
            wdg.Kick();
            --i;
        } else {
            wdg.Done();
        }
 
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
 
    // Should never reach here.
    std::cout << "Bye" << std::endl;
    return 0;
}


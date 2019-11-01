//
// Created by liu on 19-11-1.
//

#ifndef MYGC_STOP_THE_WORLD_H
#define MYGC_STOP_THE_WORLD_H

#include <pthread.h>
#include <set>


void stop_the_world(const std::set<pthread_t> &threads);
void restart_the_world();
void stop_the_world_init();

#endif //MYGC_STOP_THE_WORLD_H

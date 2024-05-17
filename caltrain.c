#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "caltrain.h"

void
station_init(struct station *station)
{
	station->waitingpassengers = 0;
	station->passengersintrain = 0;
	station->emptyseats = 0;

	pthread_mutex_init(&station->lock, NULL);
	pthread_cond_init(&station->emptychair, NULL);
	pthread_cond_init(&station->readytogo, NULL);
}

void
station_load_train(struct station *station, int count)
{
	pthread_mutex_lock(&station->lock);
	if (!(count == 0 || station->waitingpassengers == 0)){
	station->emptyseats = count;
    pthread_cond_broadcast(&station->emptychair);
	pthread_cond_wait(&station->readytogo, &station->lock);
	}
	station->emptyseats = 0;
	pthread_mutex_unlock(&station->lock);
}

void
station_wait_for_train(struct station *station)
{
	//wait for an empty chair and load train
    pthread_mutex_lock(&station->lock);
    station->waitingpassengers++;

    while(station->emptyseats == 0){
    pthread_cond_wait(&station->emptychair, &station->lock);
    }
    station->emptyseats = station->emptyseats - 1;
    station->passengersintrain = station->passengersintrain + 1;
    station->waitingpassengers = station->waitingpassengers - 1;
	pthread_mutex_unlock(&station->lock);
}

void
station_on_board(struct station *station)
{
	    pthread_mutex_lock(&station->lock);

        station->passengersintrain--;
        if ((station->emptyseats == 0 || station->waitingpassengers == 0) && station->passengersintrain == 0){
        pthread_cond_signal(&station->readytogo);
        }
        pthread_mutex_unlock(&station->lock);
}

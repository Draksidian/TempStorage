#pragma once

//we are expecting this to lag behind a lot. Maybe never even catch up with the real time scanner
#define MAX_PLAYERS_CIRCULAR_BUFFER		40000

//int HTTPPostData(int k, int x, int y, char *name, char *guild, char *guildf, int clevel, __int64 kills, int vip, int grank, __int64 might, int HasPrisoners, int plevel);

void HttpSendStartup();
void HttpSendShutdown();

void QueuePlayerToProcess(int k, int x, int y, char *name, char *guild, char *guildf, int clevel, __int64 kills, int vip, int grank, __int64 might, int StatusFlags, int plevel);
int IsHTTPQueueEmpty();

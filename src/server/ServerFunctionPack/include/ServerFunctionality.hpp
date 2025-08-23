#pragma once
#include "../InterThreadCommunicationModule/include/InterThreadCommunication.hpp"

void dbThread(DBCommunication& dbCommunication );
void handleConnection(int clientSocket,DBCommunication & dbCommunication);

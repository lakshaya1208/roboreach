#ifndef ROBOLINK_HANDLER_H
#define ROBOLINK_HANDLER_H

#include <Arduino.h>
#include "../include/config.h"

/**
 * RoboLink Communication Handler Interface
 * 
 * Manages Wi-Fi AP setup and extracts incoming control packets
 * from the RoboLink mobile application.
 */

void robolinkHandlerInit();
void robolinkHandlerUpdate();
bool robolinkIsConnected();

// Base control accessors
int  robolinkGetThrottle();
int  robolinkGetSteer();

// 2-DOF Arm control accessors
int  robolinkGetShoulder();
int  robolinkGetElbow();
int  robolinkGetGripper();

#endif // ROBOLINK_HANDLER_H
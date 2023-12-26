#ifndef GLOBAL_ENVIRONMENT_H_INCLUDED
#define GLOBAL_ENVIRONMENT_H_INCLUDED

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
//#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/vlc-channel-helper.h"
#include "ns3/vlc-device-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int UE = 7;

double room_size_x = 5;
double room_size_y = 5;
double room_size_z = 5;
double simulation_time = 10.0;

int RF_AP = 1; // RF AP number
int RF_AP_Height = 3; //RF AP height
int RF_grid_width = 1;
double RF_start_x = 0.0;
double RF_start_y = 0.0;
double RF_row_gap = 0.0;
double RF_col_gap = 0.0;

int VLC_AP = 4; //VLC AP number
int VLC_grid_width = 2; //vlc ap of every row
int VLC_AP_Height = 3; //vlc ap height
double VLC_start_x = -1.25; //vlc grid start x
double VLC_start_y = -1.25; //vlc grid start y
double VLC_row_gap = 2.5; //vlc grid row gap
double VLC_col_gap = 2.5; //vlc grid col gap

double d_ref = 10.0;//reference distance
double central_freq = 5.0e9;

double VLC_field_of_view = 90;
double VLC_PHI_half = 60;
double VLC_filter_gain = 1;
double VLC_concentrator_gain = 1;
double VLC_refractive_index = 1.5;
double VLC_receiver_area = 0.0001;
double VLC_reflect_efficiency = 0.75;

double kappa = 0.53;
double Nl = 1e-15;
double VLC_AP_Popt = 1;
double VLC_AP_Bandwidth = 20;

double RF_AP_Power = 0.1; //20dmb~=0.1w
double RF_AP_Bandwidth = 20;
double Nw = 3.1622776602e-11;//

double time_slot = 0.1;//time slot 100ms
double SSS_algo_time = 0.5;
int time_slot_count = 0;
double ILB_algo_time = 0.5;

double throughput = 0;
double ILB_throughput = 0;

double horizontalHandoverTime = 0.2;
double verticalHandoverTime = 0.5;

#endif

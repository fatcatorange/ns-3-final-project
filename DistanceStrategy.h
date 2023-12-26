#ifndef DISTANCESTRATEGY_H_INCLUDED
#define DISTANCESTRATEGY_H_INCLUDED

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cmath>
#include <ctime>
#include <chrono>
//#include <random>
#include <boost/math/distributions/rayleigh.hpp>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
//#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/vlc-channel-helper.h"
#include "ns3/vlc-device-helper.h"
#include "ns3/netanim-module.h"
#include "global_environment.h"
#include "Channel.h"
#include "AP_UE_state.h"

void printAssociateMatrix();
int checkHandover(int now_AP,int next_AP);
void printHandoverMatrix();
double countDSHH();
double countDSVH();

std::vector<int> DS_associated_matrix(UE,-1); //UE_Nodes associate matrix
std::vector<int> DS_connected_matrix(VLC_AP + RF_AP); //number of connected UE for every AP
std::vector<int> DS_HH(UE,0);
std::vector<int> DS_VH(UE,0);
std::vector<double> DS_HH_remain(UE,0);
std::vector<double> DS_VH_remain(UE,0);
std::vector<double> DS_sum_rate(UE,0);

void DS_algo()
{
    std::cout<<"-------------------------SSS algo-------------------------"<<std::endl;

    DS_connected_matrix.resize(VLC_AP + RF_AP,0);

    for(int i=0;i<DS_connected_matrix.size();i++)
    {
        DS_connected_matrix[i] = 0;
    }

    for(int i=0;i<UE;i++)//find nearest AP
    {
        int ans = 0;
        double shortest = INT_MAX;
        Ptr<Node> now_ue = UE_node_list[i]->node;
        for(int j=0;j<VLC_AP + RF_AP;j++)
        {
            Ptr<Node> now_ap = AP_node_list[j]->node;
            double distance = calculateDistance(now_ap,now_ue);
            if(distance < shortest)
            {
                shortest = distance;
                ans = j;
            }
        }
        if(checkHandover(DS_associated_matrix[i],ans) == 1)
        {
            DS_HH[i]++;
            DS_HH_remain[i] = horizontalHandoverTime;
        }
        else if(checkHandover(DS_associated_matrix[i],ans) == 2)
        {
            //if(DS_associated_matrix[i] != -1)
            {
                DS_VH_remain[i] = verticalHandoverTime;
                DS_VH[i]++;
            }
        }
        DS_associated_matrix[i] = ans;
        DS_connected_matrix[ans]++;
    }


    printAssociateMatrix();
    printHandoverMatrix();

    if(Simulator::IsFinished() == false)
    {
        Simulator::Schedule(Seconds(SSS_algo_time),&DS_algo);
    }




}

void calculate_SSS_throughput()
{
    int mod = 1.0/time_slot;
    for(int i=0;i<DS_associated_matrix.size();i++)
    {
        if(DS_VH_remain[i] <= 0.0 && DS_HH_remain[i] <= 0.0)
        {
            throughput+=(data_rate_matrix[i][DS_associated_matrix[i]]/(DS_connected_matrix[DS_associated_matrix[i]]))/(1.0/time_slot);
            DS_sum_rate[i] += (data_rate_matrix[i][DS_associated_matrix[i]]/(DS_connected_matrix[DS_associated_matrix[i]]))/(1.0/time_slot);
        }
        else
        {
            //std::cout<<"ue:"<<i<<" "<<DS_VH_remain[i]<<std::endl;
            DS_VH_remain[i]-=time_slot;
            DS_HH_remain[i]-=time_slot;
        }
    }
    //std::cout<<"SSS throughput:"<<throughput<<std::endl;
    if(time_slot_count%mod == 0)
    {
        std::cout<<"SSS throughput:"<<throughput<<std::endl;
    }
    if(Simulator::IsFinished() == false)
    {
        Simulator::Schedule(Seconds(time_slot),&calculate_SSS_throughput);
    }
}

int checkHandover(int now_AP,int next_AP)//0 = no handover 1 = HH 2 = VH
{
    if(now_AP == next_AP)
    {
        return 0;
    }
    else if(((now_AP < VLC_AP && next_AP >= VLC_AP) || (now_AP >= VLC_AP && next_AP < VLC_AP)) || now_AP == -1)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

void printAssociateMatrix()
{
    std::cout<<"SSS associateMatrix"<<std::endl;
    for(int i=0;i<DS_associated_matrix.size();i++)
    {
        std::cout<<"UE:"<<i<<" ";
        std::cout<<DS_associated_matrix[i]<<std::endl;
    }
    std::cout<<std::endl;
}

void printHandoverMatrix()
{
    std::cout<<"SSS Handover"<<std::endl;
    for(int i=0;i<DS_associated_matrix.size();i++)
    {
        std::cout<<"UE:"<<i<<" "<<"horizontal handover:";
        std::cout<<DS_HH[i]<<std::endl;
        std::cout<<"UE:"<<i<<" "<<"vertical handover:";
        std::cout<<DS_VH[i]<<std::endl;
    }
    std::cout<<std::endl;
}


double countDSHH()
{
    double temp = 0;
    for(int i=0;i<UE;i++)
    {
        temp+=DS_HH[i];
    }
    return temp/UE/simulation_time;
}

double countDSVH()
{
    double temp = 0;
    for(int i=0;i<UE;i++)
    {
        temp+=DS_VH[i];
    }
    return temp/UE/simulation_time;
}

double calculate_SSS_jains_fairness()
{
    double avg = 0;
    double bottom = 0;
    for(int i=0;i<DS_sum_rate.size();i++)
    {
        //std::cout<<DS_sum_rate[i]<<std::endl;
        avg+=DS_sum_rate[i];
        bottom+=pow(DS_sum_rate[i],2);
    }
    return (pow(avg,2))/(UE*bottom);
}
#endif // DISTANCESTRATEGY_H_INCLUDED

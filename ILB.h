#ifndef ILB_H_INCLUDED
#define ILB_H_INCLUDED

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

void ILB_DFS(int now,std::vector<int> &now_associated_matrix);
double count_now_throughput(std::vector<int> &now_associated_matrix);
void printILBAssociateMatrix();
int checkILBHandover(int now_AP,int next_AP);
void printILBHandoverMatrix();
double countILBHH();
double countILBVH();

std::vector<int> ILB_associated_matrix(UE,-1); //UE_Nodes associate matrix
std::vector<int> ILB_connected_matrix(VLC_AP + RF_AP); //number of connected UE for every AP
std::vector<int> ILB_final_connected_matrix(VLC_AP + RF_AP,-1);
std::vector<int> ILB_HH(UE,0);
std::vector<int> ILB_VH(UE,0);
std::vector<double> ILB_HH_remain(UE,0);
std::vector<double> ILB_VH_remain(UE,0);
std::vector<double> ILB_sum_rate(UE,0);
double now_max_throughput;

void ILB_method()
{
    std::cout<<"-------------------------ILB algo-------------------------"<<std::endl;

    std::vector<int> temp_associated = ILB_associated_matrix;

    for(int i=0;i<ILB_connected_matrix.size();i++)
    {
        ILB_connected_matrix[i] = 0;
    }

    now_max_throughput = 0;

    std::vector<int> now_associated_matrix(UE,-1);

    ILB_DFS(0,now_associated_matrix);

    for(int i=0;i<ILB_associated_matrix.size();i++)
    {
        if(checkILBHandover(temp_associated[i],ILB_associated_matrix[i]) == 1)
        {
            ILB_HH[i]++;
            ILB_HH_remain[i] = horizontalHandoverTime;
        }
        else if(checkILBHandover(temp_associated[i],ILB_associated_matrix[i]) == 2)
        {

            //if(temp_associated[i] != -1)
            {
                ILB_VH[i]++;
                ILB_VH_remain[i] = verticalHandoverTime;
            }
        }
    }

    printILBAssociateMatrix();
    printILBHandoverMatrix();


    if(Simulator::IsFinished() == false)
    {
        Simulator::Schedule(Seconds(ILB_algo_time),&ILB_method);
    }

}

void ILB_DFS(int now,std::vector<int> &now_associated_matrix)
{
    if(now == now_associated_matrix.size())
    {
        double temp = count_now_throughput(now_associated_matrix);
        //
        if(temp > now_max_throughput)
        {
            //std::cout<<temp<<std::endl;
            now_max_throughput = temp;
            ILB_associated_matrix = now_associated_matrix;
            ILB_final_connected_matrix = ILB_connected_matrix;
        }
        return;
    }

    for(int i=0;i<ILB_connected_matrix.size();i++)
    {
        if(data_rate_matrix[now][i] == 0)
        {
            continue;
        }
        now_associated_matrix[now] = i;
        ILB_connected_matrix[i]++;
        ILB_DFS(now+1,now_associated_matrix);
        ILB_connected_matrix[i]--;
    }

}

double count_now_throughput(std::vector<int> &now_associated_matrix)
{
    double temp = 0;
    for(int i=0;i<now_associated_matrix.size();i++)
    {
        int associated_ue = now_associated_matrix[i];
        temp+=log2((data_rate_matrix[i][associated_ue])/ILB_connected_matrix[associated_ue]);
    }
    return temp;
}

int checkILBHandover(int now_AP,int next_AP)//0 = no handover 1 = HH 2 = VH
{
    if(now_AP == next_AP)
    {
        return 0;
    }
    else if((now_AP < VLC_AP && next_AP >= VLC_AP) || (now_AP >= VLC_AP && next_AP < VLC_AP) || now_AP == -1)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

void calculate_ILB_throughput()
{
    int mod = 1.0/time_slot;
    for(int i=0;i<ILB_associated_matrix.size();i++)
    {
        if(ILB_HH_remain[i] <= 0 && ILB_VH_remain[i] <= 0)
        {
            ILB_throughput+=(data_rate_matrix[i][ILB_associated_matrix[i]]/(ILB_final_connected_matrix[ILB_associated_matrix[i]]))/(1.0/time_slot);
            ILB_sum_rate[i] += (data_rate_matrix[i][ILB_associated_matrix[i]]/(ILB_final_connected_matrix[ILB_associated_matrix[i]]))/(1.0/time_slot);
        }
        else
        {
            ILB_HH_remain[i] -= time_slot;
            ILB_VH_remain[i] -= time_slot;
        }
    }
    if(time_slot_count%mod == 0)
    {
        std::cout<<"ILB throughput:"<<ILB_throughput<<std::endl;
    }
    if(Simulator::IsFinished() == false)
    {
        Simulator::Schedule(Seconds(time_slot),&calculate_ILB_throughput);
    }
}

void printILBAssociateMatrix()
{
    std::cout<<"ILB associateMatrix"<<std::endl;
    for(int i=0;i<ILB_associated_matrix.size();i++)
    {
        std::cout<<"UE:"<<i<<" ";
        std::cout<<ILB_associated_matrix[i]<<std::endl;
    }
    std::cout<<std::endl;
}

void printILBHandoverMatrix()
{
    std::cout<<"ILB Handover"<<std::endl;
    for(int i=0;i<ILB_HH.size();i++)
    {
        std::cout<<"UE:"<<i<<" "<<"horizontal handover:";
        std::cout<<ILB_HH[i]<<std::endl;
        std::cout<<"UE:"<<i<<" "<<"vertical handover:";
        std::cout<<ILB_VH[i]<<std::endl;
    }
    std::cout<<std::endl;
}

double countILBHH()
{
    double temp = 0;
    for(int i=0;i<UE;i++)
    {
        temp+=ILB_HH[i];
    }
    return temp/UE/simulation_time;
}

double countILBVH()
{
    double temp = 0;
    for(int i=0;i<UE;i++)
    {
        temp+=ILB_VH[i];
    }
    return temp/UE/simulation_time;
}

double calculate_ILB_jains_fairness()
{
    double avg = 0;
    double bottom = 0;
    for(int i=0;i<ILB_sum_rate.size();i++)
    {
        //std::cout<<DS_sum_rate[i]<<std::endl;
        avg+=ILB_sum_rate[i];
        bottom+=pow(ILB_sum_rate[i],2);
    }
    return (pow(avg,2))/(UE*bottom);
}

#endif // ILB_H_INCLUDED

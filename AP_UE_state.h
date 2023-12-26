#ifndef AP_UE_STATE_H_INCLUDED
#define AP_UE_STATE_H_INCLUDED

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <iomanip>


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
#include "AP_Node.h"
#include "UE_Node.h"
#include "Channel.h"

void printAP_UE_Channel_gain_matrix();
double Estimate_one_VLC_SNR(int VLC_AP_Index,int UE_Index);
double Estimate_one_RF_SNR(int RF_AP_Index,int UE_index);
void calculate_AP_UE_SNR_matrix();
void printAP_UE_SNR_matrix();
void calculate_data_rate_matrix();
void print_data_rate_matrix();

std::vector<AP_node*> AP_node_list;
std::vector<UE_node*> UE_node_list;


std::vector<std::vector<double> > AP_UE_Channel_gain_matrix(UE,std::vector<double>(VLC_AP + RF_AP));
std::vector<std::vector<double> > AP_UE_SNR_matrix(UE,std::vector<double>(VLC_AP + RF_AP));
std::vector<std::vector<double>> data_rate_matrix(UE,std::vector<double>(VLC_AP + RF_AP,0));


void calculate_Channel_Gain_Matrix()
{
    for(int i=0;i<AP_UE_Channel_gain_matrix.size();i++)
    {
        for(int j=0;j<AP_UE_Channel_gain_matrix[i].size();j++)
        {
            if(j < VLC_AP)
            {
                AP_UE_Channel_gain_matrix[i][j] = Estimate_one_VLC_Channel_Gain(AP_node_list[j]->node,UE_node_list[i]->node);
            }
            else
            {
                AP_UE_Channel_gain_matrix[i][j] = Estimate_RF_Channel_Gain(AP_node_list[j]->node,UE_node_list[i]->node);
            }
        }
    }

    printAP_UE_Channel_gain_matrix();
    calculate_AP_UE_SNR_matrix();
    printAP_UE_SNR_matrix();
    calculate_data_rate_matrix();
    print_data_rate_matrix();

    time_slot_count++;
    if(Simulator::IsFinished() == false)
    {
        Simulator::Schedule(Seconds(time_slot),&calculate_Channel_Gain_Matrix);
    }
}

void printAP_UE_Channel_gain_matrix()
{
    std::cout<<std::endl;
    std::cout<<"channel_gain_matrix in time:"<<" "<<Simulator::Now().GetSeconds()<<std::endl;
    std::cout<<std::left;
    for(int i=0;i<AP_UE_Channel_gain_matrix.size();i++)
    {
        for(int j=0;j<AP_UE_Channel_gain_matrix[i].size();j++)
        {
            std::cout<<std::setw(15)<<AP_UE_Channel_gain_matrix[i][j]<<" ";
        }
        std::cout<<std::endl;
    }

}

void calculate_AP_UE_SNR_matrix()
{
    double SNR;
    for(int i=0;i<AP_UE_SNR_matrix.size();i++)
    {
        for(int j=0;j<AP_UE_SNR_matrix[i].size();j++)
        {
            if(j<VLC_AP)
            {
                SNR = Estimate_one_VLC_SNR(j,i);
            }
            else
            {
                SNR = Estimate_one_RF_SNR(j,i);
            }
            AP_UE_SNR_matrix[i][j] = SNR;
        }
    }



}

double Estimate_one_VLC_SNR(int VLC_AP_Index,int UE_Index)
{
    double noise = VLC_AP_Bandwidth * Nl ;

    double SNR = pow(kappa * VLC_AP_Popt * AP_UE_Channel_gain_matrix[UE_Index][VLC_AP_Index],2)/ (noise);

    return SNR;
}

double Estimate_one_RF_SNR(int RF_AP_Index,int UE_index)
{
    double noise = RF_AP_Bandwidth* Nw;


    double SNR = AP_UE_Channel_gain_matrix[UE_index][RF_AP_Index] * RF_AP_Power/noise;
    if(SNR == 0)
    {
        std::cout<<UE_index<<" "<<RF_AP_Index<<" "<<AP_UE_Channel_gain_matrix[UE_index][RF_AP_Index]<<std::endl;
        //printAP_UE_Channel_gain_matrix();
    }
    return SNR;
}

void printAP_UE_SNR_matrix()
{
    std::cout<<std::endl;
    std::cout<<"SNR_matrix in time:"<<" "<<Simulator::Now().GetSeconds()<<std::endl;
    std::cout<<std::left;
    for(int i=0;i<AP_UE_Channel_gain_matrix.size();i++)
    {
        for(int j=0;j<AP_UE_Channel_gain_matrix[i].size();j++)
        {
            std::cout<<std::setw(15)<<AP_UE_SNR_matrix[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}


void calculate_data_rate_matrix()
{
    for(int i=0;i<AP_UE_Channel_gain_matrix.size();i++)
    {
        for(int j=0;j<AP_UE_Channel_gain_matrix[i].size();j++)
        {
            double b;
            if(j<VLC_AP)
            {
                b = 0.5*VLC_AP_Bandwidth;
            }
            else
            {
                b = 0.5*RF_AP_Bandwidth;
            }
            data_rate_matrix[i][j] = calculate_shannon_capacity(b,AP_UE_SNR_matrix[i][j]);
        }
    }
}

void print_data_rate_matrix()
{
    std::cout<<std::endl;
    std::cout<<"data rate_matrix in time:"<<" "<<Simulator::Now().GetSeconds()<<std::endl;
    std::cout<<std::left;
    for(int i=0;i<AP_UE_Channel_gain_matrix.size();i++)
    {
        for(int j=0;j<AP_UE_Channel_gain_matrix[i].size();j++)
        {
            std::cout<<std::setw(15)<<data_rate_matrix[i][j]<<" ";
        }
    }
    std::cout<<std::endl;
}



#endif // AP_UE_STATE_H_INCLUDED

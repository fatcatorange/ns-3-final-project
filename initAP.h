#ifndef INITAP_H_INCLUDED
#define INITAP_H_INCLUDED

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
#include "global_environment.h"
#include "AP_Node.h"
#include "AP_UE_state.h"

void initRF_AP(NodeContainer &RF_AP_nodes,std::vector<AP_node*> &AP_list);
void initVLC_AP(NodeContainer &VLC_AP_nodes,std::vector<AP_node*> &AP_list);

void initVLC_AP(NodeContainer &VLC_AP_nodes,std::vector<AP_node*> &AP_list)
{
    MobilityHelper VLC_AP_mobility;
    Ptr<ListPositionAllocator> VLC_AP_pos_list = CreateObject<ListPositionAllocator>();
    int row = VLC_AP/VLC_grid_width;
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<VLC_grid_width;j++)
        {
            //std::cout<<i<<" "<<j<<std::endl;
            double inputX = VLC_start_x + (j*VLC_col_gap);
            double inputY = VLC_start_y + (i*VLC_row_gap);
            VLC_AP_pos_list->Add(Vector(inputX,inputY,VLC_AP_Height));
        }
    }
    VLC_AP_mobility.SetPositionAllocator(VLC_AP_pos_list);
    VLC_AP_mobility.Install(VLC_AP_nodes);

    std::cout<<std::endl;
    std::cout<<"VLC_AP init"<<std::endl;
    std::cout<<std::left;

    for(int i=0;i<VLC_AP;i++)
    {
        Ptr<MobilityModel> VLC_mobility_model = VLC_AP_nodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = VLC_mobility_model->GetPosition();
        AP_list.push_back(new AP_node(i,pos,VLC_AP_nodes.Get(i)));
        std::cout<<"VLC AP ID:"<<AP_list[i]->AP_ID<<std::endl;
        pos = AP_list[i]->node->GetObject<MobilityModel>()->GetPosition();
        std::cout<<"VLC AP POSITION:"<<pos.x<<" "<<pos.y<<" "<<pos.z<<std::endl;
    }

}

void initRF_AP(NodeContainer &RF_AP_nodes,std::vector<AP_node*> &AP_list){

    MobilityHelper RF_AP_mobility;
    Ptr<ListPositionAllocator> RF_AP_pos_list = CreateObject<ListPositionAllocator>();

    int row = RF_AP/RF_grid_width;
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<RF_grid_width;j++)
        {
            //std::cout<<i<<" "<<j<<std::endl;
            double inputX = RF_start_x + (j*RF_col_gap);
            double inputY = RF_start_y + (i*RF_row_gap);
            RF_AP_pos_list->Add(Vector(inputX,inputY,RF_AP_Height));
        }
    }
    RF_AP_mobility.SetPositionAllocator(RF_AP_pos_list);
    RF_AP_mobility.Install(RF_AP_nodes);

    std::cout<<std::endl;
    std::cout<<"RF_AP_Init"<<std::endl;
    std::cout<<std::left;

    for(int i=0;i<RF_AP;i++)
    {
        Ptr<MobilityModel> RF_mobility_model = RF_AP_nodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = RF_mobility_model->GetPosition();
        AP_list.push_back(new AP_node(VLC_AP + i,pos,RF_AP_nodes.Get(i)));
        std::cout<<"RF AP ID:"<<AP_list[VLC_AP + i]->AP_ID<<std::endl;
        pos = AP_list[VLC_AP + i]->node->GetObject<MobilityModel>()->GetPosition();
        std::cout<<"RF AP POSITION:"<<pos.x<<" "<<pos.y<<" "<<pos.z<<std::endl;
    }


}

#endif // INITAP_H_INCLUDED

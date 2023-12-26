#ifndef INITUE_H_INCLUDED
#define INITUE_H_INCLUDED

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <chrono>
#include <random>

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
#include "UE_Node.h"

using namespace ns3;

void initUE(NodeContainer &UE_nodes,std::vector<UE_node*> &UE_node_list);
void printUE(std::vector<UE_node*> &UE_node_list);


void initUE(NodeContainer &UE_nodes,std::vector<UE_node*> &UE_node_list)
{
    MobilityHelper UE_mobility;
    Ptr<ListPositionAllocator> UE_pos_list = CreateObject<ListPositionAllocator>();

    srand(time(NULL));

    double x = room_size_x / 2;
    double y = room_size_y / 2;

    double minx = -x;
    double maxx = x;

    double miny = -y;
    double maxy = y;

    for(int i=0;i<UE;i++)
    {
        double posX = (maxx - minx) * rand() / (RAND_MAX + 1.0) + minx;
        double posY = (maxy - miny) * rand() / (RAND_MAX + 1.0) + miny;

        UE_node_list.push_back(new UE_node(i,Vector(posX,posY,0),UE_nodes.Get(i)));
        UE_pos_list->Add(Vector(posX,posY,0));
    }


    UE_mobility.SetPositionAllocator(UE_pos_list);
    UE_mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                 "Mode",StringValue("Time"),
                                 "Time",StringValue("2.5s"),
                                 "Speed",StringValue("ns3::UniformRandomVariable[Min=0.0|Max=2]"),
                                 "Direction",StringValue("ns3::UniformRandomVariable[Min=0.0|Max=6.283184]"),
                                 "Bounds",RectangleValue(Rectangle(-x,x,-y,y)));
    UE_mobility.Install(UE_nodes);

}

void printUE(std::vector<UE_node*> &UE_node_list)
{
    std::cout<<std::endl;
    std::cout<<"print UE_AP"<<std::endl;
    std::cout<<std::left;

    std::cout<<Simulator::Now().GetSeconds()<<std::endl;
    for(int i=0;i<UE;i++)
    {
        Ptr<MobilityModel> UE_mobility_model = UE_node_list[i]->node->GetObject<MobilityModel>();;
        Vector pos = UE_mobility_model->GetPosition();
        std::cout<<" "<<"UE POSITION:"<<i<<" "<<pos.x<<" "<<pos.y<<" "<<pos.z<<std::endl;
    }

    if(Simulator::IsFinished() == false)
    {
        Simulator::Schedule(Seconds(1),&printUE,UE_node_list);
    }
}


#endif // INITUE_H_INCLUDED

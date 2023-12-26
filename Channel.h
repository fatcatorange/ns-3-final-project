#ifndef CHANNEL_H_INCLUDED
#define CHANNEL_H_INCLUDED

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

using namespace ns3;

double Get_Incidence_Angle_AP_UE(Ptr<Node> AP,Ptr<Node> UE);

std::normal_distribution<double> Gaussian(0.0,10);
boost::math::rayleigh_distribution<double> rayleigh(0.8);
std::uniform_real_distribution<double> random_p(0.0,1.0);
std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());

int random_count = 100000;
bool first_time = true;

double X = 0;
double p = 0;
double H = 0;

double RadtoDeg(double radian)
{
    return radian * 180 / M_PI;
}

double DegtoRad(double degree){
    return degree * M_PI / 180;
}

double calculateDistance(Ptr<Node> AP,Ptr<Node> UE)
{
    Ptr<MobilityModel> AP_mobility = AP->GetObject<MobilityModel>();
    Ptr<MobilityModel> UE_mobility = UE->GetObject<MobilityModel>();

    Vector AP_pos = AP_mobility->GetPosition();
    Vector UE_pos = UE_mobility->GetPosition();

    return AP_mobility->GetDistanceFrom(UE_mobility);
}

double Estimate_RF_Channel_Gain(Ptr<Node> RF_AP,Ptr<Node> UE)
{

    double d = calculateDistance(RF_AP,UE);
    //calculate the X and H that are used for this simulation by averaging 100000 random results of the corresponding distribution
    if(first_time == true)
    {
        for(int i=0;i<random_count;i++)
        {
            X+= Gaussian(gen);
            p = random_p(gen);
            H+= quantile(rayleigh,p);
        }
        X/=random_count;
        H/=random_count;
        first_time = false;
    }
    double L_d;
    // free-space path loss function
    if(d < d_ref)
        L_d = 20 * log10(central_freq * d) - 147.5;
    else
        L_d =20 * log10(central_freq * pow(d,2.75)/pow(d_ref,1.75)) - 147.5;

    double rf_channel_gain =  pow(H,2) * pow(10,((-1)*L_d+X)/10.0);

    return rf_channel_gain;
}


double Estimate_one_VLC_Channel_Gain(Ptr<Node> VLC_AP,Ptr<Node> UE){

    double incidence_angle = Get_Incidence_Angle_AP_UE(VLC_AP,UE);

    if(RadtoDeg(incidence_angle) >= VLC_field_of_view)
    {
        return 0;
    }

    double lambertian_coefficient = (-1) / (log(cos(DegtoRad(VLC_PHI_half))));

    double irradiance_angle = incidence_angle;

    Ptr<MobilityModel> VLC_AP_MobilityModel = VLC_AP->GetObject<MobilityModel>();
    Vector VLC_AP_Pos = VLC_AP_MobilityModel->GetPosition();

    Ptr<MobilityModel> UE_MobilityModel = UE->GetObject<MobilityModel>();
    Vector UE_Pos = UE_MobilityModel->GetPosition();

    double height_diff = VLC_AP_Pos.z - UE_Pos.z;

    double distance = calculateDistance(VLC_AP,UE);

    double channel_gain = VLC_receiver_area * (lambertian_coefficient + 1)/(2*M_PI*pow(distance,2));

    channel_gain = channel_gain * pow(cos(irradiance_angle),lambertian_coefficient);

    channel_gain = channel_gain * VLC_filter_gain;

    channel_gain = channel_gain * pow(VLC_refractive_index , 2) / pow(sin(DegtoRad(VLC_field_of_view)) , 2);

    channel_gain = channel_gain * cos(incidence_angle);

    return channel_gain;


}

double Get_Incidence_Angle_AP_UE(Ptr<Node> AP,Ptr<Node> UE){

    Ptr<MobilityModel> AP_MobilityModel = AP->GetObject<MobilityModel>();
    Vector AP_Pos = AP_MobilityModel->GetPosition();

    Ptr<MobilityModel> UE_MobilityModel = UE->GetObject<MobilityModel>();
    Vector UE_Pos = UE_MobilityModel->GetPosition();

    double height_diff = AP_Pos.z - UE_Pos.z;

    double dx = AP_Pos.x - UE_Pos.x;
    double dy = AP_Pos.y - UE_Pos.y;

    double plane_diff = sqrt(pow(dx,2) + pow(dy,2));

    double hypoteuse = sqrt(pow(height_diff,2) + pow(plane_diff,2));

    const double angle = acos((pow(height_diff,2) + pow(hypoteuse,2) - pow(plane_diff,2)) / (2*height_diff*hypoteuse));

    return angle;
}

double calculate_shannon_capacity(double b,double snr)
{
    if(snr == 0)
    {
        return 0;
    }
    //snr = 10*log10(snr);//to dbm?
    return b*log2(1+snr);
}
#endif // CHANNEL_H_INCLUDED

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2025 UCLAB, Chung-Ang University, Seoul, South Korea
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Thanh Thien-An Dang <attdang@uclab.re.kr>
 */

// Network topology
//
//       n0 ----------- n1
//            500 Kbps
//             5 ms

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/dash-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DashSimple");

int
main(int argc, char* argv[])
{
    //
    // Explicitly create the nodes required by the topology (shown above).
    //
    NS_LOG_INFO("Create nodes.");
    NodeContainer nodes;
    nodes.Create(2);

    NS_LOG_INFO("Create channels.");

    //
    // Explicitly create the point-to-point link required by the topology (shown above).
    //
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("500Kbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("5ms"));
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    //
    // Install the internet stack on the nodes
    //
    InternetStackHelper internet;
    internet.Install(nodes);

    //
    // We've got the "hardware" in place.  Now we need to add IP addresses.
    //
    NS_LOG_INFO("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign(devices);

    NS_LOG_INFO("Create Applications.");

    uint16_t port = 80; // well-known port number

    // Create DASH client
    DashClientHelper client("ns3::TcpSocketFactory",
                            InetSocketAddress(i.GetAddress(1), port),
                            "ns3::FdashClient");
    client.SetAttribute("VideoId", UintegerValue(1));

    ApplicationContainer clientApp = client.Install(nodes.Get(0));
    clientApp.Start(Seconds(0.25));
    clientApp.Stop(Seconds(100.0));

    // Create DASH server
    DashServerHelper server("ns3::TcpSocketFactory",
                            InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer serverApp = server.Install(nodes.Get(1));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(105.0));

    //
    // Now, do the actual simulation.
    //
    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    // Print client statistics
    Ptr<DashClient> app = DynamicCast<DashClient>(clientApp.Get(0));
    std::cout << "Client statistics:" << std::endl;
    app->GetStats();

    return 0;
}

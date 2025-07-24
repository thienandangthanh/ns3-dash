#!/usr/bin/env python3
# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
#
# Copyright (c) 2025 UCLAB, Chung-Ang University, Seoul, South Korea
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation;
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Author: Thanh Thien-An Dang <attdang@uclab.re.kr>
# Python version created from dash-simple.cc

# Network topology
#
#       n0 ----------- n1
#            500 Kbps
#             5 ms
#

try:
    from ns import ns
except ModuleNotFoundError:
    raise SystemExit(
        "Error: ns3 Python module not found;"
        " Python bindings may not be enabled"
        " or your PYTHONPATH might not be properly configured"
    )

def main():
    # Enable logging components
    # ns.LogComponentEnable("DashSimple", ns.LOG_LEVEL_INFO)

    #
    # Explicitly create the nodes required by the topology (shown above).
    #
    print("Create nodes.")
    nodes = ns.NodeContainer()
    nodes.Create(2)

    print("Create channels.")

    #
    # Explicitly create the point-to-point link required by the topology (shown above).
    #
    pointToPoint = ns.PointToPointHelper()
    pointToPoint.SetDeviceAttribute("DataRate", ns.StringValue("500Kbps"))
    pointToPoint.SetChannelAttribute("Delay", ns.StringValue("5ms"))
    devices = pointToPoint.Install(nodes)

    #
    # Install the internet stack on the nodes
    #
    internet = ns.InternetStackHelper()
    internet.Install(nodes)

    #
    # We've got the "hardware" in place.  Now we need to add IP addresses.
    #
    print("Assign IP Addresses.")
    ipv4 = ns.Ipv4AddressHelper()
    ipv4.SetBase(ns.Ipv4Address("10.1.1.0"), ns.Ipv4Mask("255.255.255.0"))
    i = ipv4.Assign(devices)

    print("Create Applications.")

    port = 80  # well-known port number

    # Create DASH client
    client = ns.DashClientHelper("ns3::TcpSocketFactory",
                                ns.InetSocketAddress(i.GetAddress(1), port).ConvertTo(),
                                "ns3::FdashClient")
    client.SetAttribute("VideoId", ns.UintegerValue(1))

    clientApp = client.Install(nodes.Get(0))
    clientApp.Start(ns.Seconds(0.25))
    clientApp.Stop(ns.Seconds(100.0))

    # Create DASH server
    server = ns.DashServerHelper("ns3::TcpSocketFactory",
                                ns.InetSocketAddress(ns.Ipv4Address.GetAny(), port).ConvertTo())
    serverApp = server.Install(nodes.Get(1))
    serverApp.Start(ns.Seconds(0.0))
    serverApp.Stop(ns.Seconds(105.0))

    #
    # Now, do the actual simulation.
    #
    print("Run Simulation.")
    ns.Simulator.Run()
    ns.Simulator.Destroy()
    print("Done.")

    # Print client statistics
    app = clientApp.Get(0)
    print("Client statistics:")
    try:
        # Try to call GetStats() if available
        app.GetStats()
    except AttributeError:
        print("GetStats() method not available in Python bindings")
        print("Client application completed successfully")

if __name__ == '__main__':
    main()

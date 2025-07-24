DASH Module Documentation
-------------------------

.. include:: replace.txt

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

The DASH (Dynamic Adaptive Streaming over HTTP) module for |ns3| provides a comprehensive simulation framework for adaptive video streaming over HTTP/TCP networks. This module enables researchers to study and evaluate different adaptive bitrate (ABR) algorithms, network conditions, and Quality of Experience (QoE) metrics in various networking scenarios.

Model Description
*****************

The source code for the DASH module lives in the directory ``contrib/dash``.

This module simulates MPEG-DASH (Moving Picture Experts Group - Dynamic Adaptive Streaming over HTTP) streaming, which is a standard for adaptive bitrate streaming that allows video content to be delivered over HTTP while adapting the video quality to changing network conditions. The module implements both client and server components, along with several state-of-the-art adaptive bitrate algorithms.

The DASH client requests video segments sequentially from a DASH server, with each segment having multiple quality representations (bitrates). Based on network conditions, buffer status, and the selected adaptation algorithm, the client dynamically switches between different bitrates to optimize the viewing experience while avoiding playback interruptions (buffer underruns).

Design
======

The DASH module follows a modular architecture that integrates seamlessly with the existing |ns3| networking stack:

**Core Components:**

- **DashClient**: The main client application that inherits from ns3::Application. It manages HTTP requests, receives video segments, maintains a playout buffer, and implements the adaptation logic.
- **DashServer**: The server application that serves video segments upon client requests. It simulates a content delivery network (CDN) or media server.
- **MpegPlayer**: Simulates the media player functionality, managing video segment playback, buffer levels, and playback state transitions.
- **HttpParser**: Handles HTTP message parsing and MPEG header processing for segment information extraction.
- **Adaptation Algorithms**: Pluggable algorithms that implement different ABR strategies (see Algorithms section).

**Helper Classes:**

- **DashClientHelper**: Simplifies client setup and configuration
- **DashServerHelper**: Simplifies server setup and configuration

The architecture supports multiple concurrent clients streaming from one or more servers, enabling the study of network congestion effects, fairness between clients, and scalability scenarios.

**Integration with ns-3:**

The module leverages |ns3|'s socket infrastructure (TCP), application framework, attribute system, and logging/tracing capabilities. It works with any |ns3| network topology and can be combined with mobility models, error models, and traffic control mechanisms.

Scope and Limitations
=====================

**What the model can do:**

- Simulate adaptive HTTP video streaming with multiple clients and servers
- Evaluate different ABR algorithms under various network conditions
- Model buffer dynamics, startup delays, and playback interruptions
- Generate QoE metrics including average bitrate, number of quality switches, and stall events
- Support different video profiles and segment durations
- Work with wireless, wired, and mixed network topologies
- Integrate with |ns3|'s mobility, propagation, and traffic models

**What the model cannot do:**

- Provide pixel-level video quality assessment (focuses on bitrate adaptation)
- Simulate actual video decoding or rendering
- Model advanced DASH features like tiled streaming or multi-view video
- Support live streaming scenarios (focuses on Video-on-Demand)
- Implement DRM or security aspects of video streaming

**Limitations:**

- Video segments are represented abstractly by their size and bitrate, not actual video data
- The model assumes perfect HTTP/TCP reliability (no application-layer retransmissions)
- Limited to single-layer video encoding (no spatial or temporal scalability)

References
==========

The DASH module was developed based on research presented in the following publications:

- Dimitrios J. Vergados, Angelos Michalas, Aggeliki Sgora, Dimitrios D. Vergados, and Periklis Chatzimisios. "FDASH: A Fuzzy-Based MPEG/DASH Adaptation Algorithm." IEEE Systems Journal 10, no. 2 (2016): 859-868.

- Dimitrios J. Vergados, Angelos Michalas, Aggeliki Sgora, and Dimitrios D. Vergados. "A fuzzy controller for rate adaptation in MPEG-DASH clients." In 2014 IEEE 25th Annual International Symposium on Personal, Indoor, and Mobile Radio Communication (PIMRC), pp. 2008-2012. IEEE, 2014.

- Dimitrios J. Vergados, Angelos Michalas, Aggeliki Sgora, and Dimitrios D. Vergados. "A control-based algorithm for rate adaption in MPEG-DASH." In Information, Intelligence, Systems and Applications, IISA 2014, The 5th International Conference on, pp. 438-442. IEEE, 2014.

Additional algorithm implementations are based on:

- **AAASH**: K. Miller, E. Quacchio, G. Gennari, and A. Wolisz, "Adaptation algorithm for adaptive streaming over HTTP," in Proc. 19th Int. IEEE PV Workshop, 2012, pp. 173–178.
- **OSMF**: R. K. P. Mok, X. Luo, E. W. W. Chan, and R. K. C. Chang, "QDASH: A QoE-aware DASH system," in Proc. 3rd MMSys Conf., New York, NY, USA, 2012, pp. 11–22.
- **SVAA**: G. Tian and Y. Liu, "Towards agile and smooth video adaptation in dynamic HTTP streaming," in Proc. 8th Int. CoNEXT, New York, NY, USA, 2012, pp. 109–120.
- **RAAHS**: C. Liu, I. Bouazizi, and M. Gabbouj, "Rate adaptation for adaptive HTTP streaming," in Proc. 2nd Annu. ACM Conf. MMSys, New York, NY, USA, 2011, pp. 169–174.
- **SFTM**: C. Liu, I. Bouazizi, M. M. Hannuksela, and M. Gabbouj, "Rate adaptation for dynamic adaptive streaming over HTTP in content distribution network," Signal Process., Image Commun., vol. 27, no. 4, pp. 288–311, Apr. 2012.

Usage
*****

This section describes how to use the DASH module in your |ns3| simulations, covering basic setup, configuration options, and advanced scenarios.

Building the DASH Module
=========================

The DASH module is designed to work with |ns3| version 3.30 and later. To build the module:

1. Ensure the module is placed in the ``contrib/dash`` directory
2. Reconfigure |ns3| with examples enabled::

    ./ns3 configure --enable-examples --enable-tests

3. Build the project::

    ./ns3 build

4. Verify the installation by running a basic example::

    ./ns3 run 'dash-example --users=1 --algorithms=ns3::FdashClient'

Helpers
=======

The DASH module provides helper classes that simplify application setup and configuration:

**DashClientHelper:**

The client helper manages DASH client application installation and configuration::

    DashClientHelper clientHelper ("ns3::TcpSocketFactory", serverAddress);
    clientHelper.SetAttribute ("VideoId", UintegerValue (1));
    clientHelper.SetAttribute ("TargetDt", TimeValue (Seconds (35.0)));
    clientHelper.SetAttribute ("window", TimeValue (Seconds (10.0)));
    clientHelper.SetAttribute ("bufferSpace", UintegerValue (30000000));

    ApplicationContainer clientApps = clientHelper.Install (clientNodes);

**DashServerHelper:**

The server helper sets up DASH servers::

    DashServerHelper serverHelper (serverPort);
    ApplicationContainer serverApps = serverHelper.Install (serverNodes);

**Algorithm Selection:**

Different ABR algorithms can be selected by specifying the algorithm type::

    // Using FDASH algorithm
    DashClientHelper clientHelper ("ns3::FdashClient", serverAddress);

    // Using AAASH algorithm
    DashClientHelper clientHelper ("ns3::AaashClient", serverAddress);

Attributes
==========

The DASH module exposes several key attributes for configuration:

**DashClient Attributes:**

- **VideoId** (UintegerValue, default: 0): Identifier for the video content being streamed
- **Remote** (AddressValue): Server address for segment requests
- **Protocol** (TypeIdValue, default: TcpSocketFactory): TCP socket factory type
- **TargetDt** (TimeValue, default: 35s): Target buffer duration for adaptation decisions
- **window** (TimeValue, default: 10s): Averaging window for throughput estimation
- **bufferSpace** (UintegerValue, default: 30000000): Client buffer size in bytes

**Usage Example:**

::

    clientHelper.SetAttribute ("TargetDt", TimeValue (Seconds (30.0)));
    clientHelper.SetAttribute ("bufferSpace", UintegerValue (50000000));
    clientHelper.SetAttribute ("window", TimeValue (Seconds (15.0)));

Output
======

The DASH module generates comprehensive output data for performance analysis:

**Trace Sources:**

- **Tx**: Fired when a new HTTP request packet is transmitted
- Various internal traces for segment requests, quality decisions, and buffer events

**Logging Output:**

The module provides detailed logging through several log components:

- **DashClient**: Core client operation logging
- Algorithm-specific logging for each ABR implementation

Enable logging with::

    NS_LOG=DashClient=level_info ./ns3 run dash-example

**Performance Metrics:**

The simulation automatically outputs key QoE metrics:

- **Average Bitrate**: Mean video quality over the session
- **Bitrate Changes**: Number of quality switches
- **Buffer Interruptions**: Stall events and total interruption time
- **Average Buffer Level**: Mean buffer occupancy during playback

**Example Output:**

::

    ns3::FdashClient-Node: 0 InterruptionTime: 0 interruptions: 0 avgRate: 248101 minRate: 89000 AvgDt: 38.4632 changes: 7
    ns3::FdashClient-Node: 1 InterruptionTime: 0 interruptions: 0 avgRate: 175772 minRate: 89000 AvgDt: 37.577 changes: 5

Adaptation Algorithms
====================

The DASH module implements six state-of-the-art ABR algorithms:

**FDASH (Fuzzy-based DASH):**
A fuzzy logic controller that considers buffer level, throughput estimate, and segment fetch time to make quality decisions. Provides smooth adaptation with good QoE.

**AAASH (Advanced Adaptive Algorithm for Streaming over HTTP):**
Uses a buffer-based approach with aggressive bitrate increases when buffer is full and conservative decreases when buffer is low.

**OSMF (Open Source Media Framework):**
A threshold-based algorithm that switches bitrates based on buffer occupancy levels and throughput estimates.

**SVAA (Smooth Video Adaptation Algorithm):**
Focuses on minimizing quality oscillations while maintaining good buffer health through smoothed quality transitions.

**RAAHS (Rate Adaptation Algorithm for HTTP Streaming):**
Simple throughput-based algorithm that selects the highest sustainable bitrate based on measured throughput.

**SFTM (Segment Fetch Time Minimization):**
Optimizes segment fetch time while considering buffer levels and available bitrates.

**Algorithm Selection:**

Each algorithm can be selected during client helper configuration::

    DashClientHelper clientHelper ("ns3::FdashClient", serverAddress);  // FDASH
    DashClientHelper clientHelper ("ns3::AaashClient", serverAddress);  // AAASH
    DashClientHelper clientHelper ("ns3::OsmpClient", serverAddress);   // OSMF
    DashClientHelper clientHelper ("ns3::SvaaClient", serverAddress);   // SVAA
    DashClientHelper clientHelper ("ns3::RaahsClient", serverAddress);  // RAAHS
    DashClientHelper clientHelper ("ns3::SftmClient", serverAddress);   // SFTM

Advanced Usage
==============

**Multiple Quality Profiles:**

The module supports different video profiles through the VideoId attribute. Each video can have different segment sizes and available bitrates.

**Network Condition Studies:**

Combine with |ns3|'s link models to study adaptation under various conditions::

    // Variable bandwidth link
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));

**Mobility Scenarios:**

For wireless scenarios, combine with mobility models::

    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel");
    mobility.Install (clientNodes);

**Large-Scale Simulations:**

The module supports multiple concurrent clients for studying fairness and network congestion::

    NodeContainer clients;
    clients.Create (50);  // 50 concurrent clients

    for (uint32_t i = 0; i < clients.GetN(); ++i) {
        DashClientHelper clientHelper (algorithm, serverAddress);
        ApplicationContainer app = clientHelper.Install (clients.Get(i));
        app.Start (Seconds (i * 0.1));  // Staggered starts
    }

Examples
========

The module includes several example scripts demonstrating different usage patterns:

**dash-example.cc:**
Basic client-server setup with configurable parameters. Good starting point for understanding the module::

    ./ns3 run 'dash-example --users=3 --algorithms=ns3::FdashClient --linkRate=1000Kbps'

**dash-wifi.cc:**
Demonstrates DASH streaming over WiFi networks with mobility::

    ./ns3 run dash-wifi

**link-example.cc:**
Studies the impact of different link characteristics on streaming performance::

    ./ns3 run link-example

**lena-dash.cc:**
Complex LTE network scenario combining DASH streaming with LTE/EPC simulation.

**dash-cttc-qos.cc:**
Advanced QoS evaluation scenario with detailed performance analysis.

Troubleshooting
===============

**Common Issues:**

- **Compilation Errors**: Ensure you have |ns3| version 3.30 or later and all dependencies installed
- **Low Performance**: For large simulations, consider reducing logging verbosity and using optimized build
- **Buffer Underruns**: If experiencing frequent stalls, increase buffer size or reduce target buffer time
- **Algorithm Selection**: Ensure the algorithm string exactly matches the class name (e.g., "ns3::FdashClient")

**Debug Tips:**

- Enable detailed logging to understand algorithm decisions: ``NS_LOG=DashClient=level_debug``
- Use |ns3|'s built-in tracing to monitor TCP performance and packet flows
- Check buffer levels and segment fetch times to diagnose adaptation issues

**Performance Optimization:**

- Use release builds for large simulations: ``./ns3 configure --build-profile=optimized``
- Reduce unnecessary logging in large-scale scenarios
- Consider simulation time limits for long-running scenarios

Validation
**********

The DASH module has been extensively validated through several approaches:

**Academic Validation:**

The module has been used to reproduce results from multiple peer-reviewed publications, demonstrating its accuracy in modeling adaptive streaming behaviors and network interactions.

**Algorithm Verification:**

Each implemented ABR algorithm has been validated against reference implementations and published performance results, ensuring correct adaptation logic and parameter handling.

**Test Suite:**

The module includes a comprehensive test suite (``dash-test-suite.cc``) that covers:

- Basic client-server functionality
- HTTP message parsing and segment handling
- Buffer management and playback simulation
- Algorithm integration and switching logic

**Code Coverage:**

The test suite provides significant code coverage of the core functionality, including:

- Client application lifecycle (start, segment requests, adaptation decisions, stop)
- Server request handling and response generation
- MPEG player buffer management and state transitions
- HTTP parser message processing

**Continuous Integration:**

The module is designed to integrate with |ns3|'s continuous integration framework, ensuring compatibility across different platforms and |ns3| versions.

**Usage Validation:**

The module has been successfully used in numerous research studies and student projects, validating its usability and research applicability in diverse scenarios including:

- QoE optimization in mobile networks
- CDN performance evaluation
- Network neutrality studies
- 5G network slicing for video services

Run the test suite with::

    ./test.py --suite=dash

The successful execution of all tests confirms proper module installation and functionality.

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 TEI of Western Macedonia, Greece
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
 * Author: Dimitrios J. Vergados <djvergad@gmail.com>
 */

#ifndef DASH_SERVER_H
#define DASH_SERVER_H

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"

#include <map>
#include <queue>

namespace ns3
{

class Address;
class Socket;
class Packet;

/**
 * @ingroup dash
 *
 * @brief DASH server application for NS-3 simulations
 *
 * This application serves as a DASH (Dynamic Adaptive Streaming over HTTP) server
 * that responds to client requests for video segments. It receives requests from
 * DASH clients and transmits back synthetic MPEG video segments.
 *
 * The server operates by:
 * - Listening for incoming TCP connections from DASH clients
 * - Parsing HTTP requests containing video segment parameters (VideoId, Resolution, SegmentId)
 * - Generating synthetic video segments with realistic timing and size characteristics
 * - Transmitting video frames back to clients with proper HTTP and MPEG headers
 *
 * The constructor specifies the Address (IP address and port) and the
 * transport protocol to use. A virtual receive method is installed
 * as a callback on the receiving socket.
 *
 * @note This implementation generates synthetic video data rather than using
 *       real video content, making it suitable for network simulation purposes.
 */
class DashServer : public Application
{
  public:
    /**
     * @brief Get the TypeId for DashServer class
     * @return The object TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * @brief Default constructor
     */
    DashServer();

    /**
     * @brief Destructor
     */
    virtual ~DashServer();

    /**
     * @brief Get the listening socket for the server
     * @return Pointer to the listening socket
     */
    Ptr<Socket> GetListeningSocket(void) const;

    /**
     * @brief Get the list of accepted client sockets
     * @return List of pointers to accepted socket connections
     */
    std::list<Ptr<Socket>> GetAcceptedSockets(void) const;

  protected:
    /**
     * @brief Dispose of the server resources
     *
     * Cleans up sockets and socket list when the server is destroyed.
     */
    virtual void DoDispose(void);

  private:
    /**
     * @brief Start the DASH server application
     *
     * Creates and configures the listening socket, sets up callbacks
     * for handling incoming connections and data. Supports both
     * unicast and multicast addresses.
     */
    virtual void StartApplication(void);

    /**
     * @brief Stop the DASH server application
     *
     * Closes all accepted client connections and the listening socket,
     * cleaning up all network resources.
     */
    virtual void StopApplication(void);

    /**
     * @brief Handle incoming data from clients
     * @param socket The socket that received data
     *
     * Processes incoming HTTP requests from DASH clients. Parses the
     * custom HTTP header to extract video segment request parameters
     * (VideoId, Resolution, SegmentId) and responds by sending the
     * requested video segment.
     */
    void HandleRead(Ptr<Socket> socket);

    /**
     * @brief Handle socket ready for sending data
     * @param socket The socket ready for transmission
     * @param unused Unused parameter (required by callback signature)
     *
     * Transmits queued video frames to the client. Handles TCP flow control
     * by checking available send buffer space and fragmenting large frames
     * if necessary to avoid blocking.
     */
    void DataSend(Ptr<Socket> socket, uint32_t unused);

    /**
     * @brief Generate and send a video segment to a client
     * @param video_id The ID of the video being streamed
     * @param resolution The requested bitrate/resolution in bits per second
     * @param segment_id The sequential ID of the segment within the video
     * @param socket The client socket to send the segment to
     *
     * Creates a synthetic video segment consisting of multiple MPEG frames.
     * Each frame has:
     * - Random size based on the requested bitrate (assumes 50 fps)
     * - HTTP header with response metadata
     * - MPEG header with frame timing and metadata
     * - Synthetic payload data
     *
     * The segment is queued for transmission and sent when the socket is ready.
     *
     * @note This implementation generates synthetic data rather than using
     *       real video content, making it suitable for network simulation.
     */
    void SendSegment(uint32_t video_id,
                     uint32_t resolution,
                     uint32_t segment_id,
                     Ptr<Socket> socket);

    /**
     * @brief Handle new client connections
     * @param s The new accepted socket
     * @param from The address of the connecting client
     *
     * Sets up callbacks for the newly accepted client socket and
     * adds it to the list of active connections.
     */
    void HandleAccept(Ptr<Socket> s, const Address& from);

    /**
     * @brief Handle client connection close (normal)
     * @param socket The socket that was closed
     */
    void HandlePeerClose(Ptr<Socket> socket);

    /**
     * @brief Handle client connection close (error)
     * @param socket The socket that had an error
     */
    void HandlePeerError(Ptr<Socket> socket);

    Ptr<Socket> m_socket;                //!< Listening socket
    std::list<Ptr<Socket>> m_socketList; //!< List of accepted sockets

    Address m_local;    //!< Local address to bind to
    uint32_t m_totalRx; //!< Total bytes received
    TypeId m_tid;       //!< Protocol TypeId

    /**
     * @brief Trace source for received packets
     */
    TracedCallback<Ptr<const Packet>, const Address&> m_rxTrace;

    /**
     * @brief Queue of generated MPEG frames for each client socket
     *
     * Maps each client socket to a queue of video frames waiting to be transmitted.
     * Used to handle TCP flow control and ensure frames are sent in order.
     */
    std::map<Ptr<Socket>, std::deque<Packet>> m_queues;

    Ptr<Packet> m_pending_packet = nullptr; //!< Buffer for incomplete received packets
};

} // namespace ns3

#endif /* DASH_SERVER_H */

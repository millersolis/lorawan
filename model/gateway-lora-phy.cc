/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of Padova
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
 * Author: Davide Magrin <magrinda@dei.unipd.it>
 */

#include "ns3/gateway-lora-phy.h"
#include "ns3/lora-tag.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("GatewayLoraPhy");

NS_OBJECT_ENSURE_REGISTERED (GatewayLoraPhy);

/**************************************
 *    ReceptionPath implementation    *
 **************************************/
GatewayLoraPhy::ReceptionPath::ReceptionPath (double frequencyMHz) :
  m_frequencyMHz (frequencyMHz),
  m_available (1),
  m_event (0),
  m_endReceiveEventId (EventId ())
{
  NS_LOG_FUNCTION_NOARGS ();
}

GatewayLoraPhy::ReceptionPath::~ReceptionPath (void)
{
  NS_LOG_FUNCTION_NOARGS ();
}

double
GatewayLoraPhy::ReceptionPath::GetFrequency (void)
{
  return m_frequencyMHz;
}

bool
GatewayLoraPhy::ReceptionPath::IsAvailable (void)
{
  return m_available;
}

void
GatewayLoraPhy::ReceptionPath::Free (void)
{
  m_available = true;
  m_event = 0;
  m_endReceiveEventId = EventId ();
}

void
GatewayLoraPhy::ReceptionPath::LockOnEvent (Ptr<LoraInterferenceHelper::Event>
                                            event)
{
  m_available = false;
  m_event = event;
}

void
GatewayLoraPhy::ReceptionPath::SetEvent (Ptr<LoraInterferenceHelper::Event>
                                         event)
{
  m_event = event;
}

Ptr<LoraInterferenceHelper::Event>
GatewayLoraPhy::ReceptionPath::GetEvent (void)
{
  return m_event;
}

void
GatewayLoraPhy::ReceptionPath::SetFrequency (double frequencyMHz)
{
  m_frequencyMHz = frequencyMHz;
}

EventId
GatewayLoraPhy::ReceptionPath::GetEndReceive (void)
{
  return m_endReceiveEventId;
}

void
GatewayLoraPhy::ReceptionPath::SetEndReceive (EventId endReceiveEventId)
{
  m_endReceiveEventId = endReceiveEventId;
}

/***********************************************************************
 *                 Implementation of Gateway methods                   *
 ***********************************************************************/

TypeId
GatewayLoraPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GatewayLoraPhy")
    .SetParent<LoraPhy> ()
    .SetGroupName ("lorawan")
    .AddTraceSource ("NoReceptionBecauseTransmitting",
                     "Trace source indicating a packet "
                     "could not be correctly received because"
                     "the GW is in transmission mode",
                     MakeTraceSourceAccessor
                       (&GatewayLoraPhy::m_noReceptionBecauseTransmitting),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("LostPacketBecauseNoMoreReceivers",
                     "Trace source indicating a packet "
                     "could not be correctly received because"
                     "there are no more demodulators available",
                     MakeTraceSourceAccessor
                       (&GatewayLoraPhy::m_noMoreDemodulators),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("OccupiedReceptionPaths",
                     "Number of currently occupied reception paths",
                     MakeTraceSourceAccessor
                       (&GatewayLoraPhy::m_occupiedReceptionPaths),
                     "ns3::TracedValueCallback::Int");
  return tid;
}

GatewayLoraPhy::GatewayLoraPhy () :
  m_isTransmitting (false)
{
  NS_LOG_FUNCTION_NOARGS ();
}

GatewayLoraPhy::~GatewayLoraPhy ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

// Uplink sensitivity (Source: SX1276 datasheet)
// {SF7, SF8, SF9, SF10, SF11, SF12}
const double GatewayLoraPhy::sensitivity[6] =
{-123, -126, -129, -132, -134, -136};

void
GatewayLoraPhy::AddReceptionPath (double frequencyMHz)
{
  NS_LOG_FUNCTION (this << frequencyMHz);

  m_receptionPaths.push_back (Create<GatewayLoraPhy::ReceptionPath>
                                (frequencyMHz));
}

void
GatewayLoraPhy::ResetReceptionPaths (void)
{
  NS_LOG_FUNCTION (this);

  m_receptionPaths.clear ();
}

void
GatewayLoraPhy::TxFinished (Ptr<Packet> packet)
{
  m_isTransmitting = false;
}

bool
GatewayLoraPhy::IsTransmitting (void)
{
  return m_isTransmitting;
}

bool
GatewayLoraPhy::IsOnFrequency (double frequencyMHz)
{
  NS_LOG_FUNCTION (this << frequencyMHz);

  // Search every demodulator to see whether there's one listening on this
  // frequency.
  std::list< Ptr< GatewayLoraPhy::ReceptionPath > >::iterator it;

  for (it = m_receptionPaths.begin (); it != m_receptionPaths.end (); ++it)
    {
      Ptr<GatewayLoraPhy::ReceptionPath> currentPath = *it;

      NS_LOG_DEBUG ("Current reception path is on frequency " <<
                    currentPath->GetFrequency ());

      if (currentPath->GetFrequency () == frequencyMHz)
        {
          return true;
        }
    }
  return false;
}
}
}

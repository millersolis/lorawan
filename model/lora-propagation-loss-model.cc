/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
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
 * Author: Miller Solis <msolissa@sfu.ca>
 */

#include "ns3/lora-propagation-loss-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include <cmath>

namespace ns3 {
namespace lorawan {

NS_LOG_COMPONENT_DEFINE ("LoraPropagationLossModel");

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (LoraPropagationLossModel);

TypeId 
LoraPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LoraPropagationLossModel")
    .SetParent<PropagationLossModel> ()
    .SetGroupName ("LoraPropagation")
    // No default constructor added since class has pure virtual methods
    // .AddConstructor<LoraPropagationLossModel> ()
    // .AddAttribute ("Variable", "The SF used to pick a loss every time CalcRxPower is invoked.",
    //               DoubleValue (7),
    //               MakeDoubleAccessor (&LoraPropagationLossModel::m_txSF),
    //               MakeDoubleChecker<double> ())
;
  return tid;
}

LoraPropagationLossModel::LoraPropagationLossModel ()
  : PropagationLossModel ()
{
 // Important: SF has to be updated before using the model
}

LoraPropagationLossModel::LoraPropagationLossModel (uint8_t sf)
{
  m_txSF = sf;
}

LoraPropagationLossModel::~LoraPropagationLossModel ()
{
}

void LoraPropagationLossModel::SetTxSF (uint8_t sf)
{
  m_txSF = sf;
}

uint8_t LoraPropagationLossModel::GetTxSF () const
{
  return m_txSF;
}

double LoraPropagationLossModel::DoCalcRxPower (double txPowerDbm,
                        Ptr<MobilityModel> a,
                        Ptr<MobilityModel> b) const
{
  // TODO: assert sf
  return DoCalcRxPower(txPowerDbm, m_txSF, a, b);
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (RYLRLoraPropagationLossModel);

TypeId 
RYLRLoraPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RYLRLoraPropagationLossModel")
    .SetParent<LoraPropagationLossModel> ()
    .SetGroupName ("LoraPropagation")
    .AddConstructor<RYLRLoraPropagationLossModel> ()
  ;
  return tid;
}

RYLRLoraPropagationLossModel::RYLRLoraPropagationLossModel ()
  : LoraPropagationLossModel ()
{
    // Important: SF has to be updated before using the model 
}

RYLRLoraPropagationLossModel::RYLRLoraPropagationLossModel (uint8_t txSF)
  : LoraPropagationLossModel (txSF)
{
}

RYLRLoraPropagationLossModel::~RYLRLoraPropagationLossModel ()
{
}

double
RYLRLoraPropagationLossModel::DoCalcRxPower (double txPowerDbm,
                                            uint8_t txSF,
                                            Ptr<MobilityModel> a,
                                            Ptr<MobilityModel> b) const
{

  double distance = a->GetDistanceFrom (b);
  if (distance <= 0)
    distance = -distance;

  double pathLossDb;
  double minPathLossDb;
  double maxPathLossDb;
  double antennaLossDb;
  double maxRange;  // in meters

  if (txSF >= 7 && txSF <= 12) {
    NS_LOG_DEBUG ("SF="<<txSF<<" not supported, using SF 7 to calculate rxPower");
    txSF = 7;
  }

// TODO: Use actual data for SF 7,8,9,10,12 [Miller]
  if (txSF == 7) {
    minPathLossDb = 49 + 14;       // RYLR experimental max RSSI was -49 for 14 txPowerDbm
    maxPathLossDb = 114 + 14;      // RYLR experimental min RSSI was -106 for 14 txPowerDbm
    antennaLossDb = 123 - 114;     // SX1276 sensitivity -134, for RYLR sensitivity -106
    maxRange = 500;                // RYLR experimental distance for maxPathLossDb
  }
  else if (txSF == 8) {
    minPathLossDb = 49 + 14;       // RYLR experimental max RSSI was -49 for 14 txPowerDbm
    maxPathLossDb = 112 + 14;      // RYLR experimental min RSSI was -106 for 14 txPowerDbm
    antennaLossDb = 126 - 112;     // SX1276 sensitivity -134, for RYLR sensitivity -106
    maxRange = 675;                // RYLR experimental distance for maxPathLossDb
  }
  else if (txSF == 9) {
    minPathLossDb = 49 + 14;       // RYLR experimental max RSSI was -49 for 14 txPowerDbm
    maxPathLossDb = 110 + 14;      // RYLR experimental min RSSI was -106 for 14 txPowerDbm
    antennaLossDb = 129 - 110;     // SX1276 sensitivity -134, for RYLR sensitivity -106
    maxRange = 850;                // RYLR experimental distance for maxPathLossDb
  }
  else if (txSF == 10) {
    minPathLossDb = 49 + 14;       // RYLR experimental max RSSI was -49 for 14 txPowerDbm
    maxPathLossDb = 108 + 14;      // RYLR experimental min RSSI was -106 for 14 txPowerDbm
    antennaLossDb = 132 - 108;     // SX1276 sensitivity -134, for RYLR sensitivity -106
    maxRange = 1025;                // RYLR experimental distance for maxPathLossDb
  }
  else if (txSF == 11) {
    // TODO: Verify data
    minPathLossDb = 49 + 14;       // RYLR experimental max RSSI was -49 for 14 txPowerDbm
    maxPathLossDb = 106 + 14;      // RYLR experimental min RSSI was -106 for 14 txPowerDbm
    antennaLossDb = 134 - 106;     // SX1276 sensitivity -134, for RYLR sensitivity -106
    maxRange = 1200;                // RYLR experimental distance for maxPathLossDb, was found to be ~600
  }
  else if (txSF == 12) {
    minPathLossDb = 49 + 14;       // RYLR experimental max RSSI was -49 for 14 txPowerDbm
    maxPathLossDb = 104 + 14;      // RYLR experimental min RSSI was -106 for 14 txPowerDbm
    antennaLossDb = 136 - 104;     // SX1276 sensitivity -134, for RYLR sensitivity -106
    maxRange = 1375;                // RYLR experimental distance for maxPathLossDb
  }
 
  // TODO: Find better function to approximate loss with real data [Miller]
  // Linear approx
  pathLossDb = (((maxPathLossDb - minPathLossDb)/(maxRange - 0)) * distance) + minPathLossDb;
  // Make up for antenna loss from RYLR to compare with sensitivity values of SX1276 for reception
  double rxc = - pathLossDb + antennaLossDb;
  NS_LOG_DEBUG ("distance="<<distance<<"m, "<< "attenuation coefficient="<<rxc<<"db");  // Add SF?
  return txPowerDbm + rxc;
}

int64_t
RYLRLoraPropagationLossModel::DoAssignStreams (int64_t stream)
{
  return 1;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (RandomLoraPropagationLossModel);

TypeId 
RandomLoraPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RandomLoraPropagationLossModel")
    .SetParent<LoraPropagationLossModel> ()
    .SetGroupName ("LoraPropagation")
    .AddConstructor<RandomLoraPropagationLossModel> ()
    .AddAttribute ("Variable", "The random variable used to pick a loss every time CalcRxPower is invoked.",
                   StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                   MakePointerAccessor (&RandomLoraPropagationLossModel::m_variable),
                   MakePointerChecker<RandomVariableStream> ())
  ;
  return tid;
}

RandomLoraPropagationLossModel::RandomLoraPropagationLossModel ()
  : LoraPropagationLossModel ()
{
    // Important: SF has to be updated before using the model 
}

RandomLoraPropagationLossModel::RandomLoraPropagationLossModel (uint8_t txSF)
  : LoraPropagationLossModel (txSF)
{
}

RandomLoraPropagationLossModel::~RandomLoraPropagationLossModel ()
{
}

double
RandomLoraPropagationLossModel::DoCalcRxPower (double txPowerDbm,
                                           uint8_t txSF,  // TODO: Use for calculation [Miller]
                                           Ptr<MobilityModel> a,
                                           Ptr<MobilityModel> b) const
{
  double rxc = -m_variable->GetValue ();
  NS_LOG_DEBUG ("attenuation coefficient="<<rxc<<"Db");
  return txPowerDbm + rxc;
}

int64_t
RandomLoraPropagationLossModel::DoAssignStreams (int64_t stream)
{
  m_variable->SetStream (stream);
  return 1;
}

}
}
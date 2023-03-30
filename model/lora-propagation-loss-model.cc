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
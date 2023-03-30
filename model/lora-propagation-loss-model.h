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

#ifndef LORA_PROPAGATION_LOSS_MODEL_H
#define LORA_PROPAGATION_LOSS_MODEL_H

#include "ns3/propagation-loss-model.h"
#include "ns3/object.h"
#include "ns3/random-variable-stream.h"
#include <map>

namespace ns3 {

/**
 * \defgroup propagation Propagation Models
 *
 */

class MobilityModel;
class PropagationLossModel;

namespace lorawan {
/**
 * \ingroup propagation
 *
 * \brief The propagation loss class for lora
 */ 
class LoraPropagationLossModel : public PropagationLossModel
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  LoraPropagationLossModel ();
  LoraPropagationLossModel (uint8_t sf);
  virtual ~LoraPropagationLossModel ();

  void SetTxSF (uint8_t sf);
  uint8_t GetTxSF () const;

private:
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  LoraPropagationLossModel (const LoraPropagationLossModel &);
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   * \returns
   */
  LoraPropagationLossModel & operator = (const LoraPropagationLossModel &);

  /**
   * To be implemented by all classes who inherit
   * from LoraPropagationLossModel (i.e. propagation loss models for lora)
   */
  virtual double DoCalcRxPower (double txPowerDbm,
                                uint8_t txSF,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const = 0;

  /**
   * Ensures lora models who inherit form this class can be simply used as
   * a regular propagation loss model
   */
  double DoCalcRxPower (double txPowerDbm,
                        Ptr<MobilityModel> a,
                        Ptr<MobilityModel> b) const override;

  virtual int64_t DoAssignStreams (int64_t stream) = 0;

  uint8_t m_txSF = 7; // Important: SF has to be updated before using the model 
  // Bandwidth is assummed to be 125kHz throughout the whole lorawan module
};

/**
 * \ingroup propagation
 *
 * \brief The propagation loss follows a probablity distribution based on distance
 * and SF (Spreading Factor) for the RYLR896 using SX1276
 */

class RYLRLoraPropagationLossModel : public LoraPropagationLossModel
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  RYLRLoraPropagationLossModel ();
  RYLRLoraPropagationLossModel (uint8_t txSF);
  virtual ~RYLRLoraPropagationLossModel ();

private:
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  RYLRLoraPropagationLossModel (const RYLRLoraPropagationLossModel &);
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   * \returns
   */
  RYLRLoraPropagationLossModel & operator = (const RYLRLoraPropagationLossModel &);
  virtual double DoCalcRxPower (double txPowerDbm,
                                uint8_t txSF,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  virtual int64_t DoAssignStreams (int64_t stream);
};

/**
 * \ingroup propagation
 *
 * \brief Random propagation loss
 */ 
class RandomLoraPropagationLossModel : public LoraPropagationLossModel
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  RandomLoraPropagationLossModel ();
  RandomLoraPropagationLossModel (uint8_t txSF);
  virtual ~RandomLoraPropagationLossModel ();

private:
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  RandomLoraPropagationLossModel (const RandomLoraPropagationLossModel &);
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   * \returns
   */
  RandomLoraPropagationLossModel & operator = (const RandomLoraPropagationLossModel &);
  virtual double DoCalcRxPower (double txPowerDbm,
                                uint8_t txSF,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  virtual int64_t DoAssignStreams (int64_t stream);
  Ptr<RandomVariableStream> m_variable; //!< random generator
};

} // namespace lorawan
} // namespace ns3

#endif /* LORA_PROPAGATION_LOSS_MODEL_H */

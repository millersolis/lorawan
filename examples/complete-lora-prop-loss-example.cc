/*
 * This script simulates a complete LoRaWAN scenario with a single gateway and
 * multiple end devices. Metrics of interest are printed in the standard output.
 */

#include "ns3/end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/end-device-lora-mac.h"
#include "ns3/gateway-lora-mac.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/lora-helper.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/double.h"
#include "ns3/random-variable-stream.h"
#include "ns3/periodic-sender-helper.h"
#include "ns3/command-line.h"
#include "ns3/network-server-helper.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/lora-propagation-loss-model.h"
#include "ns3/correlated-shadowing-propagation-loss-model.h"
#include "ns3/building-penetration-loss.h"
#include "ns3/building-allocator.h"
#include "ns3/buildings-helper.h"
#include "ns3/forwarder-helper.h"
#include <algorithm>
#include <ctime>

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE ("CompleteLorawanNetworkExample");

// Network settings
int nDevices = 200;
int nGateways = 1;
double radius = 6300;

// Channel model
bool addBuildings = false;

int appPeriodSeconds = 600;
int periodsToSimulate = 1;

// Output control
bool print = true;

int main (int argc, char *argv[])
{

  CommandLine cmd;
  cmd.AddValue ("nDevices",
                "Number of end devices to include in the simulation",
                nDevices);
  cmd.AddValue ("addBuildings",
                "Whether to employ a realistic channel model "
                "featuring shadowing and buildings",
                addBuildings);
  cmd.AddValue ("radius",
                "The radius of the area to simulate",
                radius);
  cmd.AddValue ("appPeriod",
                "The period in seconds to be used by periodically "
                "transmitting applications",
                appPeriodSeconds);
  cmd.AddValue ("MType", "ns3::EndDeviceLoraMac::MType");
  cmd.AddValue ("print",
                "Whether or not to print various info",
                print);
  cmd.Parse (argc, argv);

  // Set up logging
  LogComponentEnable ("CompleteLorawanNetworkExample", LOG_LEVEL_ALL);
  LogComponentEnable("LoraChannel", LOG_LEVEL_INFO);
  LogComponentEnable("LoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("EndDeviceLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("GatewayLoraPhy", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraInterferenceHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraMac", LOG_LEVEL_ALL);
  // LogComponentEnable("EndDeviceLoraMac", LOG_LEVEL_ALL);
  // LogComponentEnable("GatewayLoraMac", LOG_LEVEL_ALL);
  // LogComponentEnable("LogicalLoraChannelHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LogicalLoraChannel", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraPhyHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraMacHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("PeriodicSenderHelper", LOG_LEVEL_ALL);
  // LogComponentEnable("PeriodicSender", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraMacHeader", LOG_LEVEL_ALL);
  // LogComponentEnable("LoraFrameHeader", LOG_LEVEL_ALL);
  LogComponentEnable ("LoraPropagationLossModel", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkScheduler", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkServer", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkStatus", LOG_LEVEL_ALL);
  // LogComponentEnable("NetworkController", LOG_LEVEL_ALL);

  /***********
   *  Setup  *
   ***********/

  // Create the time value from the period
  Time appPeriod = Seconds (appPeriodSeconds);

  // Mobility
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
                                 "rho", DoubleValue (radius),
                                 "X", DoubleValue (0.0),
                                 "Y", DoubleValue (0.0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  /************************
   *  Create the channel  *
   ************************/

  // Create the lora channel object
  Ptr<RYLRLoraPropagationLossModel> loss = CreateObject<RYLRLoraPropagationLossModel> ();

  if (addBuildings)
    {
      // Create the correlated shadowing component
      Ptr<CorrelatedShadowingPropagationLossModel> shadowing = CreateObject<CorrelatedShadowingPropagationLossModel> ();

      // Aggregate shadowing to the logdistance loss
      loss->SetNext (shadowing);

      // Add the effect to the channel propagation loss
      Ptr<BuildingPenetrationLoss> buildingLoss = CreateObject<BuildingPenetrationLoss> ();

      shadowing->SetNext (buildingLoss);
    }

  Ptr<PropagationDelayModel> delay =
    CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<LoraChannel> channel = CreateObject<LoraChannel> (loss, delay);

  /************************
   *  Create the helpers  *
   ************************/

  // Create the LoraPhyHelper
  LoraPhyHelper phyHelper = LoraPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the LoraMacHelper
  LoraMacHelper macHelper = LoraMacHelper ();

  // Create the LoraHelper
  LoraHelper helper = LoraHelper ();
  helper.EnablePacketTracking ("performance.txt"); // Output filename

  //Create the NetworkServerHelper
  NetworkServerHelper nsHelper = NetworkServerHelper ();

  //Create the ForwarderHelper
  ForwarderHelper forHelper = ForwarderHelper ();

  /************************
   *  Create End Devices  *
   ************************/

  // Create a set of nodes
  NodeContainer endDevices;
  endDevices.Create (nDevices);

  // Assign a mobility model to each node
  mobility.Install (endDevices);

  // Create the LoraNetDevices of the end devices
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<LoraDeviceAddressGenerator> addrGen = CreateObject<LoraDeviceAddressGenerator> (nwkId,nwkAddr);

  // Create the LoraNetDevices of the end devices
  macHelper.SetAddressGenerator (addrGen);
  phyHelper.SetDeviceType (LoraPhyHelper::ED);
  macHelper.SetDeviceType (LoraMacHelper::ED);
  helper.Install (phyHelper, macHelper, endDevices);
  // Now end devices are connected to the channel

  /*********************
   *  Create Gateways  *
   *********************/

  // Create the gateway nodes (allocate them uniformely on the disc)
  NodeContainer gateways;
  gateways.Create (nGateways);

  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  // Make it so that nodes are at a certain height > 0
  allocator->Add (Vector (0.0, 0.0, 15.0));
  allocator->Add (Vector (0.0, 3000.0, 15.0));
  allocator->Add (Vector (3000.0, 0.0, 15.0));
  allocator->Add (Vector (0.0, -3000.0, 15.0));
  allocator->Add (Vector (-3000.0, 0.0, 15.0));
  mobility.SetPositionAllocator (allocator);
  mobility.Install (gateways);

  // Create a netdevice for each gateway
  phyHelper.SetDeviceType (LoraPhyHelper::GW);
  macHelper.SetDeviceType (LoraMacHelper::GW);
  helper.Install (phyHelper, macHelper, gateways);

  /**********************
   *  Handle buildings  *
   **********************/

  double xLength = 130;
  double deltaX = 32;
  double yLength = 64;
  double deltaY = 17;
  int gridWidth = 2 * radius / (xLength + deltaX);
  int gridHeight = 2 * radius / (yLength + deltaY);
  if (addBuildings == false)
    {
      gridWidth = 0;
      gridHeight = 0;
    }
  Ptr<GridBuildingAllocator> gridBuildingAllocator;
  gridBuildingAllocator = CreateObject<GridBuildingAllocator> ();
  gridBuildingAllocator->SetAttribute ("GridWidth", UintegerValue (gridWidth));
  gridBuildingAllocator->SetAttribute ("LengthX", DoubleValue (xLength));
  gridBuildingAllocator->SetAttribute ("LengthY", DoubleValue (yLength));
  gridBuildingAllocator->SetAttribute ("DeltaX", DoubleValue (deltaX));
  gridBuildingAllocator->SetAttribute ("DeltaY", DoubleValue (deltaY));
  gridBuildingAllocator->SetAttribute ("Height", DoubleValue (6));
  gridBuildingAllocator->SetBuildingAttribute ("NRoomsX", UintegerValue (2));
  gridBuildingAllocator->SetBuildingAttribute ("NRoomsY", UintegerValue (4));
  gridBuildingAllocator->SetBuildingAttribute ("NFloors", UintegerValue (2));
  gridBuildingAllocator->SetAttribute ("MinX", DoubleValue (-gridWidth * (xLength + deltaX) / 2 + deltaX / 2));
  gridBuildingAllocator->SetAttribute ("MinY", DoubleValue (-gridHeight * (yLength + deltaY) / 2 + deltaY / 2));
  BuildingContainer bContainer = gridBuildingAllocator->Create (gridWidth * gridHeight);

  BuildingsHelper::Install (endDevices);
  BuildingsHelper::Install (gateways);
  BuildingsHelper::MakeMobilityModelConsistent ();

  // Print the buildings
  if (print)
    {
      std::ofstream myfile;
      myfile.open ("buildings.txt");
      std::vector<Ptr<Building> >::const_iterator it;
      int j = 1;
      for (it = bContainer.Begin (); it != bContainer.End (); ++it, ++j)
        {
          Box boundaries = (*it)->GetBoundaries ();
          myfile << "set object " << j << " rect from " << boundaries.xMin << "," << boundaries.yMin << " to " << boundaries.xMax << "," << boundaries.yMax << std::endl;
        }
      myfile.close ();

    }

  /**********************************************
   *  Set up the end device's spreading factor  *
   **********************************************/
  macHelper.SetSpreadingFactorsUp (endDevices, gateways, channel);

  /*********************************************
   *  Install applications on the end devices  *
   *********************************************/

  Time appStopTime = appPeriod;
  PeriodicSenderHelper appHelper = PeriodicSenderHelper ();
  appHelper.SetPeriod (Seconds (appPeriodSeconds));
  appHelper.SetPacketSize (23);
  ApplicationContainer appContainer = appHelper.Install (endDevices);

  appContainer.Start (Seconds (0));
  appContainer.Stop (appStopTime);

  /**************************
   *  Create Network Server  *
   ***************************/

  // Create the NS node
  NodeContainer networkServer;
  networkServer.Create (1);

  // Create a NS for the network
  nsHelper.SetEndDevices (endDevices);
  nsHelper.SetGateways (gateways);
  nsHelper.Install (networkServer);

  //Create a forwarder for each gateway
  forHelper.Install (gateways);

  /**********************
   * Print output files *
   *********************/
  if (print)
    {
      helper.PrintEndDevices (endDevices, gateways,
                              "endDevices.dat");
    }

  ////////////////
  // Simulation //
  ////////////////

  Simulator::Stop (appStopTime + Hours (1));

  NS_LOG_INFO ("Running simulation...");
  Simulator::Run ();

  Simulator::Destroy ();

  ///////////////////////////
  // Print results to file //
  ///////////////////////////
  NS_LOG_INFO ("Computing performance metrics...");
  helper.PrintPerformance (Seconds (0), appStopTime + Hours(1));

  return 0;
}

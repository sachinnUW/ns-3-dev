#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream> 
#include <utility> 
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <math.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"

  /*
   * Network topology:
   *
   *      |     + --------------------------------------------------------->
   *      |     UE
   *      |
   *      |           d          d         d          d    
   *    y |     |----------x----------x----------x----------x----------
   *      |     |       eNodeB      eNodeB    eNodeB     eNodeB
   *      |   d |
   *      |     |
   *      |     |                                             d = distance
   *            o (0, 0, 0)                                   y = yForUe
   */


using namespace ns3;

class LenaHandoverSimple{

private: 

  /**
   * \param s The UE IMSI
   * Prints UE position in (X, Y) after locating it in one of the eNBs.
  */
  void PrintUePosition (uint64_t imsi);

  // Constant variables for simulation
  const uint16_t m_numberOfUes;
  const uint16_t m_numberOfEnbs;
  const uint16_t m_numBearersPerUe;
  const double m_distance; 		// m
  const double m_yForUe;   		// m
  const double m_speed;    		// m/s
  const double m_enbTxPowerDbm; 	// dB
  const double m_simTime;  		// s

  // Specifics for RLFs
  const double m_qOut;			// dB
  const double m_qIn;			// dB
  const double m_numQoutEvalSf;		// 1 subframe = 1ms
  const double m_numQinEvalSf;		// 1 subframe = 1ms
  const uint16_t m_t310;		// ms
  const uint16_t m_n310;
  const uint16_t m_n311;
  const bool m_useIdealRrc;
  const bool m_usePdsch;

  // Variables for RLF indication tracking
  uint16_t m_counterN310FirsteNB;
  Time m_t310StartTimeFirstEnb;

  // Specifics for HO algo
  const std::string m_hoAlgo;
  const double m_hyst;
  const uint16_t m_ttt;

  // LTE specific objects
  Ptr<LteHelper> m_lteHelper;
  Ptr<PointToPointEpcHelper> m_epcHelper;
  Ptr<Node> m_pgw;
  NodeContainer m_remoteHostContainer;
  Ptr<Node> m_remoteHost;  

  // Internet specific objects
  InternetStackHelper m_internet;
  PointToPointHelper m_p2ph;
  NetDeviceContainer m_internetDevices;
  Ipv4AddressHelper m_ipv4h;
  Ipv4InterfaceContainer m_internetIpIfaces;
  Ipv4Address m_remoteHostAddr;
  Ipv4StaticRoutingHelper m_ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> m_remoteHostStaticRouting;
  Ipv4InterfaceContainer m_ueIpIfaces;

  // UE and eNB specific objects
  NodeContainer m_ueNodes;
  NodeContainer m_enbNodes;
  MobilityHelper m_enbMobility;
  MobilityHelper m_ueMobility;
  NetDeviceContainer m_enbLteDevs;
  NetDeviceContainer m_ueLteDevs;

  // Callback functions
  void NotifyConnectionEstablishedUe (uint64_t imsi, uint16_t cellid, uint16_t rnti);
  void NotifyHandoverStartUe (uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);   
  void NotifyHandoverEndOkUe (uint64_t imsi, uint16_t cellid, uint16_t rnti);
  void NotifyConnectionEstablishedEnb (uint64_t imsi, uint16_t cellid, uint16_t rnti);
  void NotifyHandoverStartEnb (uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);
  void NotifyHandoverEndOkEnb (uint64_t imsi, uint16_t cellid, uint16_t rnti); 
  void NotifyRecvMeasurementReport (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteRrcSap::MeasurementReport report);
  void NotifyUeMeasurements (uint16_t imsi, uint16_t cellId, double rsrpDbm, double rsrqDbm, bool servingCell, uint8_t ccId);
  void UeStateTransition (uint64_t imsi, uint16_t cellId, uint16_t rnti, LteUeRrc::State oldState, LteUeRrc::State newState);
  void EnbRrcTimeout (uint64_t imsi, uint16_t rnti, uint16_t cellId, std::string cause);
  void NotifyConnectionReleaseAtEnodeB (uint64_t imsi, uint16_t cellId, uint16_t rnti);
  void PhySyncDetection (uint64_t imsi, uint16_t rnti, uint16_t cellId, std::string type, uint8_t count);
  void RadioLinkFailure (uint64_t imsi, uint16_t cellId, uint16_t rnti);
  void NotifyRandomAccessErrorUe (uint64_t imsi, uint16_t cellId, uint16_t rnti);
  void NotifyConnectionTimeoutUe (uint64_t imsi, uint16_t cellId, uint16_t rnti, uint8_t connEstFailCount);
  void NotifyRaResponseTimeoutUe (uint64_t imsi, bool contention, uint8_t preambleTxCounter, uint8_t maxPreambleTxLimit);

  // Functions for configuration
  void ConfigureSettings();
  void ConfigureCallbacks();
  void ConfigureLogging();
  bool SetRrc(std::string useRealRrc = "true");
  bool SetSINRCalc(std::string sinrFromPdcchOnly = "true");

  // Functions for setting up the node based network
  void SetupLTE();
  void SetChannelModel();  
  void SetMobility();
  void SetupInternet();
  void InstallUDP(uint16_t dlPort = 10000, uint16_t ulPort = 20000);
  
  // Streams for outputting stuff from callbacks
  std::ofstream m_ueMeasurements;

public:

  LenaHandoverSimple(uint16_t numUes, uint16_t numEnbs, uint16_t numBearers, double dist, double y, double speed, double TxPower, double simTime = 0.0, double qOut = -5, double qIn = -3.8, uint16_t numQoutEvalSf = 20, uint16_t numQinEvalSf = 10, uint16_t t310 = 50, uint16_t n310 = 1, uint16_t n311 = 1, std::string useRealRrc = "true", std::string sinrFromPdcchOnly = "true", std::string hoAlgo = "A3Rsrp", double hysteresis = 3, uint16_t timeToTrigger = 256):
	
	// Simulation specific parameters
        m_numberOfUes(numUes), m_numberOfEnbs(numEnbs), m_numBearersPerUe(numBearers), m_distance(dist), m_yForUe(y), m_speed(speed), m_enbTxPowerDbm(TxPower), 

	// Set simTime if defined, else calculate based on the speed.
        m_simTime((simTime != 0.0) ? simTime : (double)(numEnbs + 1) * dist / speed),

	// RLF specific parameters
	m_qOut(qOut), m_qIn(qIn), m_numQoutEvalSf(numQoutEvalSf), m_numQinEvalSf(numQinEvalSf), m_t310(t310), m_n310(n310), m_n311(n311), m_useIdealRrc(SetRrc(useRealRrc)), m_usePdsch(SetSINRCalc(sinrFromPdcchOnly)),

	// RLF specific variables
	m_counterN310FirsteNB(0), m_t310StartTimeFirstEnb (Seconds(0)),

	// HO algo specific parameters
	m_hoAlgo(hoAlgo), m_hyst(hysteresis), m_ttt(timeToTrigger) {

	// Perform the initial configuration settings.	
	ConfigureSettings();

	// Setup LTE network, set channel models, set mobility models and start the internet.
	SetupLTE();
        SetChannelModel();
	SetMobility();
	SetupInternet();

	// Add callbacks and enable logging.
	ConfigureCallbacks();	
	ConfigureLogging();

          
  }

  void Run();

};

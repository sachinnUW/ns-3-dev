#include "lena-handover-simple.h"

using namespace ns3;

// Map each of UE RRC states to its string representation.
static const std::string g_ueRrcStateName[LteUeRrc::NUM_STATES] =
{

  "IDLE_START",
  "IDLE_CELL_SEARCH",
  "IDLE_WAIT_MIB_SIB1",
  "IDLE_WAIT_MIB",
  "IDLE_WAIT_SIB1",
  "IDLE_CAMPED_NORMALLY",
  "IDLE_WAIT_SIB2",
  "IDLE_RANDOM_ACCESS",
  "IDLE_CONNECTING",
  "CONNECTED_NORMALLY",
  "CONNECTED_HANDOVER",
  "CONNECTED_PHY_PROBLEM",
  "CONNECTED_REESTABLISHING"

};

/**
 * \param s the UE RRC state
 * \return the string representation of the given state
*/
static const std::string & ToString (LteUeRrc::State s) {

  return g_ueRrcStateName[s];

}

/**
 * \param inStr the input string
 * \return the lower case representation of the given string
*/
std::string ToLower (std::string inStr) {

  std::string outStr = inStr;
  for(uint8_t i = 0; i < outStr.size (); i++)
    outStr[i] = std::tolower (outStr[i]);

  return outStr;

}

/**
 * \param imsi, cell, rnti reported from 'LteEnbRrc/RecvMeasurementReport' event
 * Callback to display measurement report received at eNB with time
*/
void
LenaHandoverSimple::NotifyRecvMeasurementReport (uint64_t imsi,
						 uint16_t cellId,
                             			 uint16_t rnti,
                             			 LteRrcSap::MeasurementReport report)
{

  std::cout << "UE_RECV_MEAS_REPORT: IMSI-" << imsi
            << " CellId-" << cellId
	    << " Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

}

/**
 * \param imsi, cellId, rsrpDbm, rsrqDbm, servingCell, ccId reported from 'LteUePhy/ReportUeMeasurements' event
 * Callback to output measurement report sent from UE to eNB into a log file
*/
void
LenaHandoverSimple::NotifyUeMeasurements (uint16_t imsi,
	  				  uint16_t cellId,
 					  double rsrpDbm,
					  double rsrqDbm,
					  bool servingCell,
					  uint8_t ccId)
{

  m_ueMeasurements << std::setw (7) << std::setprecision (3) << std::fixed << Simulator::Now ().GetSeconds () << " " 
		   << std::setw (4) << imsi << " "
    		   << std::setw (7) << cellId << " " 
    		   << std::setw (7) << (servingCell ? "1" : "0") << " " 
    		   << std::setw (20) << rsrpDbm << " " 
    		   << std::setw (15) << rsrqDbm << std::endl;

}

/**
 * \param imsi, cellId, rnti reported from 'LteUeRrc/ConnectionEstablished' event
 * Callback to display connection establishment of UE with time
*/
void
LenaHandoverSimple::NotifyConnectionEstablishedUe (uint64_t imsi,
                               			   uint16_t cellId,
                               			   uint16_t rnti)
{

  std::cout << "UE_EST_CONNECTION: IMSI-" << imsi << " "
            << "CellId-" << cellId << " "
            << "RNTI-" << rnti << " "
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

}

/**
 * \param imsi, cellId, rnti, targetCellId reported from 'LteUeRrc/HandoverStart' event
 * Callback to display handover start message reception at UE with time
*/
void
LenaHandoverSimple::NotifyHandoverStartUe (uint64_t imsi,
                       			   uint16_t cellId,
                       			   uint16_t rnti,
                       			   uint16_t targetCellId)
{

  std::cout << "UE_HO_START: IMSI-" << imsi << " "
            << "SCellId-" << cellId << " "
            << "TCellId-" << targetCellId << " "
            << "RNTI-" << rnti << " "
            << "Time-" << Simulator::Now ().GetSeconds ()            
            << std::endl;

}


/**
 * \param imsi, cellId, rnti reported from 'LteUeRrc/HandoverEndOk' event
 * Callback to display successful handover end message reception at UE with time
*/
void
LenaHandoverSimple::NotifyHandoverEndOkUe (uint64_t imsi,
                       			   uint16_t cellId,
                       			   uint16_t rnti)
{

  std::cout << "UE_HO_END: IMSI-" << imsi << " "
            << "TCellId-" << cellId << " "
            << "RNTI-" << rnti << " "
            << "Time-" << Simulator::Now ().GetSeconds ()            
            << std::endl;

}

/**
 * \param imsi, cellId, rnti reported from 'LteEnbRrc/ConnectionEstablished' event
 * Callback to connection establishment of UE at eNB with time
*/
void
LenaHandoverSimple::NotifyConnectionEstablishedEnb (uint64_t imsi,
   			                            uint16_t cellId,
                                		    uint16_t rnti)
{

  std::cout << "ENB_EST_CONNECTION: IMSI-" << imsi << " "
            << "CellId-" << cellId << " "
            << "RNTI-" << rnti << " "
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

  // For the case, when there is only one eNB with ideal RRC, a UE might reconnects to the eNB multiple times due to more than
  // one RLF. To handle this, we reset the counter here so, even if the UE connects multiple time to cell 1 we count N310
  // indication correctly, i.e., for each RLF UE RRC should receive configured number of N310 indications.
  if (cellId == 1)
    m_counterN310FirsteNB = 0;
    
}


/**
 * \param imsi, cellId, rnti, targetCellId reported from 'LteEnbRrc/HandoverStart' event
 * Callback to display handover start message reception at eNB with time
*/
void
LenaHandoverSimple::NotifyHandoverStartEnb (uint64_t imsi,
                        		    uint16_t cellId,
                        		    uint16_t rnti,
                        		    uint16_t targetCellId)
{

  std::cout << "ENB_START_HO: IMSI-" << imsi << " "
            << "SCellId-" << cellId << " "
            << "TCellId-" << targetCellId << " "
            << "RNTI-" << rnti << " "           
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

}

/**
 * \param imsi, cellId, rnti reported from 'LteEnbRrc/HandoverEndOk' event
 * Callback to display successful handover end message reception at eNB with time
*/
void
LenaHandoverSimple::NotifyHandoverEndOkEnb (uint64_t imsi,
                        		    uint16_t cellId,
	                          	    uint16_t rnti)
{

  std::cout << "ENB_END_HO: IMSI-" << imsi << " " 
            << "TCellId-" << cellId << " " 
            << "RNTI-" << rnti << " "
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

}

/**
   * \param s The UE IMSI
   * Prints UE position in (X, Y) after locating it in one of the eNBs.
*/
void
LenaHandoverSimple::PrintUePosition (uint64_t imsi)
{

  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it) {

    Ptr<Node> node = *it;
    int nDevs = node->GetNDevices ();
    for (int j = 0; j < nDevs; j++) {

      Ptr<LteUeNetDevice> uedev = node->GetDevice (j)->GetObject <LteUeNetDevice> ();
      if (uedev)
        if (imsi == uedev->GetImsi ()) {
    
          Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
          std::cout << "IMSI-" << uedev->GetImsi () << " X-" << pos.x << " Y-" << pos.y << " ";

        }
    
    }
  
  }

}

/**
 * \param imsi, cellId, rnti, oldState, newState reported from 'LteUeRrc/StateTransition' event
 * Callback to display RRC state transition at UE with time
*/
void
LenaHandoverSimple::UeStateTransition (uint64_t imsi,
	                   	       uint16_t cellId,
				       uint16_t rnti,
				       LteUeRrc::State oldState,
				       LteUeRrc::State newState)
{

  std::cout << "UE_STATE_CHANGE: IMSI-" << imsi << " " 
            << "CellId-" << cellId << " " 
            << "RNTI-" << rnti << " "
	    << "OldState-" << ToString (oldState) << " "
	    << "NewState-" << ToString (newState) << " "
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

}

/**
 * \param imsi, rnti, cellId, cause reported from 'LteEnbRrc/RrcTimeout' event
 * Callback to display RRC timeout at eNB with time
*/
void
LenaHandoverSimple::EnbRrcTimeout (uint64_t imsi,
				   uint16_t rnti,
				   uint16_t cellId,
				   std::string cause)
{

  std::cout << "ENB_RRC_TIMEOUT: IMSI-" << imsi << " "
            << "RNTI-" << rnti << " "
            << "CellId-" << cellId << " "
            << "Cause-" << cause << " "
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

}

/**
 * \param imsi, cellId, rnti reported from 'LteEnbRrc/NotifyConnectionRelease' event
 * Callback to display UE connection release at eNB with time
*/
void
LenaHandoverSimple::NotifyConnectionReleaseAtEnodeB (uint64_t imsi,
						     uint16_t cellId,
						     uint16_t rnti)
{

  std::cout << "UE_CONTEXT_DEST: IMSI-" << imsi << " "
            << "RNTI-" << rnti << " "
            << "CellId-" << cellId << " "
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;  

}

/**
 * \param imsi, rnti, cellId, type, count reported from 'LteUeRrc/PhySyncDetection' event
 * Callback to display reception of in-sync/out-of-sync indications at UE with time
*/
void 
LenaHandoverSimple::PhySyncDetection (uint64_t imsi,
				      uint16_t rnti,
				      uint16_t cellId,
				      std::string type,
				      uint8_t count)
{

  if(type == "Notify out of sync")
    std::cout << "OUT_OF_SYNC: ";
  else
    std::cout << "IN_SYNC: ";

  std::cout << "IMSI-" << imsi << " "
            << "RNTI-" << rnti << " "
            << "CellId-" << cellId << " " 
	    << "Count-" << +count << " "
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

  if (type == "Notify out of sync" && cellId == 1) {

    ++m_counterN310FirsteNB;
    if (m_counterN310FirsteNB == m_n310)
      m_t310StartTimeFirstEnb = Simulator::Now ();

  }

}

/**
 * \param imsi, cellId, rnti reported from 'LteUeRrc/RadioLinkFailure' event
 * Callback to display RLF detection with time and position at UE
*/
void 
LenaHandoverSimple::RadioLinkFailure (uint64_t imsi,
				      uint16_t cellId,
				      uint16_t rnti)
{

  std::cout << "RLF_DETECTED: ";
  PrintUePosition (imsi);
  std::cout << "RNTI-" << rnti << " "
            << "CellId-" << cellId << " " 
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;     

}

/**
 * \param imsi, cellId, rnti reported from 'LteUeRrc/RandomAccessError' event
 * Callback to display RACH error at UE with time
*/
void
LenaHandoverSimple::NotifyRandomAccessErrorUe (uint64_t imsi,
					       uint16_t cellId,
					       uint16_t rnti)
{

  std::cout << "UE_RACH_FAILED: IMSI-" << imsi << " "
	    << "RNTI-" << rnti << " "
            << "CellId-" << cellId << " " 
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl;

}

/**
 * \param imsi, cellId, rnti, connEstFailCount reported from 'LteUeRrc/ConnectionTimeout' event
 * Callback to display connection establishment timeout at UE
*/
void
LenaHandoverSimple::NotifyConnectionTimeoutUe (uint64_t imsi,
					       uint16_t cellId,
					       uint16_t rnti,
	                        	       uint8_t connEstFailCount)
{

  std::cout << "UE_RRC_TIMEOUT: IMSI-" << imsi << " "
	    << "RNTI-" << rnti << " "
            << "CellId-" << cellId << " "
	    << "T300_ExpCount-" << (uint16_t) connEstFailCount << " " 
            << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl; 
}

/**
 * \param imsi, contention, preambleTxCounter, maxPreambleTxLimit reported from 'LteUeMac/RaResponseTimeout' event
 * Callback to display RACH timeout at UE with time
*/
void
LenaHandoverSimple::NotifyRaResponseTimeoutUe (uint64_t imsi,
					       bool contention,
                           		       uint8_t preambleTxCounter,
                           		       uint8_t maxPreambleTxLimit)
{

  std::cout << "UE_RA_RESPONSE_TIMEOUT: IMSI-" << imsi << " "
	    << "ContentionFlag-" << contention << " "
            << "PreambleTxCounter-" << (uint16_t) preambleTxCounter << " "
            << "MaxPreambleTxLimit-" << (uint16_t) maxPreambleTxLimit << " "
	    << "Time-" << Simulator::Now ().GetSeconds ()
            << std::endl; 

}

/**
 * \param UE node, interval for displaying events
 * Function that schedules itself periodically

void
LenaHandoverSimple::TracePosition (Ptr<Node> ue, Time interval)
{
  Vector v = ue->GetObject<MobilityModel> ()->GetPosition ();
  m_positionTrace << std::setw (7) << std::setprecision (3) << std::fixed << Simulator::Now ().GetSeconds () << " " 
    << v.x << " " << v.y << std::endl;
  Simulator::Schedule (interval, &LenaHandoverSimple::TracePosition, ue, interval);
}*/

/**
 * \param context packet, address, receiver reported from "$ns3::PacketSink/RxWithAddresses"
 * Callback to display UE position periodically
*/
void
LenaHandoverSimple::NotifyPacketSinkRx (Ptr<const Packet> packet,
					const Address &address,
					const Address &receiver)
{

  m_packetSinkRx << std::setw (7) << std::setprecision (3) << std::fixed << Simulator::Now ().GetSeconds () << " "
		 << std::setw (5) << packet->GetSize () 
		 << std::setw (5) << " " << receiver 
		 << std::endl;

}

/*
 * \param context
 * Parse context strings of the form "/NodeList/9/DeviceList/9/Mac/Assoc" to extract the NodeId.
*/
uint32_t
LenaHandoverSimple::ContextToNodeId (std::string context)
{

  std::string sub = context.substr (10);  // skip "/NodeList/"
  uint32_t pos = sub.find ("/Device");
  return atoi (sub.substr (0,pos).c_str ());

}

/*
 * \param context, rnti, cqi reported from "$ns3::RrFfMacScheduler/WidebandCqiReport"
 * Parse context strings of the form "/NodeList/9/DeviceList/9/Mac/Assoc" to extract the NodeId
*/
void
LenaHandoverSimple::NotifyCqiReport (std::string context,
				     uint16_t rnti,
				     uint8_t cqi)
{

  m_cqiTrace << std::setw (7) << std::setprecision (3) << std::fixed << Simulator::Now ().GetSeconds () << " "
    	     << std::setw (4) << ContextToNodeId (context) << " "
    	     << std::setw (8) << rnti << " " 
    	     << std::setw (4) << static_cast<uint16_t> (cqi) 
	     << std::endl;

}


/*
 * \param oldValue, newValue reported from "CongState"
 * Callback to display the congestion state of the TCP
*/
void
LenaHandoverSimple::CongStateTrace (const TcpSocketState::TcpCongState_t oldValue, const TcpSocketState::TcpCongState_t newValue)
{

  m_tcpCongStateTrace << std::setw (7) << std::setprecision (3) << std::fixed << Simulator::Now ().GetSeconds () << " "
    		      << std::setw (4) << TcpSocketState::TcpCongStateName[oldValue] << " "
    		      << std::setw (4) << TcpSocketState::TcpCongStateName[newValue] << std::endl;

}

// Function to connect TCP trace; this callback is connected in this manner cos it needs to start late.
void
LenaHandoverSimple::ConnectTcpTrace (void)
{

  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/CongState", MakeCallback (&LenaHandoverSimple::CongStateTrace, this));

}

// Function to configure logging for ns-3 components and open log files
void
LenaHandoverSimple::ConfigureLogging () {

  // LogLevel logLevel = (LogLevel)(LOG_PREFIX_ALL | LOG_LEVEL_ALL);
  // LogComponentEnable ("m_lteHelper", logLevel);
  // LogComponentEnable ("m_epcHelper", logLevel);
  // LogComponentEnable ("EpcEnbApplication", logLevel);
  // LogComponentEnable ("EpcMmeApplication", logLevel);
  // LogComponentEnable ("Epcm_pgwApplication", logLevel);
  // LogComponentEnable ("EpcSgwApplication", logLevel);
  // LogComponentEnable ("EpcX2", logLevel);
  // LogComponentEnable ("LteEnbRrc", logLevel);
  // LogComponentEnable ("LteEnbNetDevice", logLevel);
  // LogComponentEnable ("LteUeRrc", logLevel);
  // LogComponentEnable ("LteUeNetDevice", logLevel);
  // LogComponentEnable ("A2A4RsrqHandoverAlgorithm", logLevel);
  // LogComponentEnable ("A3RsrpHandoverAlgorithm", logLevel);
  // Uncomment to enable PCAP tracing
  // m_p2ph.EnablePcapAll("lena-x2-handover-measures");  	
  // m_lteHelper->EnableLogComponents();

  // Enable traces at all levels.
  m_lteHelper->EnableRlcTraces ();
  m_lteHelper->EnablePdcpTraces ();
  Ptr<RadioBearerStatsCalculator> rlcStats = m_lteHelper->GetRlcStats ();
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
  Ptr<RadioBearerStatsCalculator> pdcpStats = m_lteHelper->GetPdcpStats ();
  pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1.0)));
  m_lteHelper->EnableMacTraces ();
  m_lteHelper->EnablePhyTraces ();

  // Open logging files for additional experiment logging.
  OpenCustomLogs ();
  	
}

// Function to open custom log files
void
LenaHandoverSimple::OpenCustomLogs () {

  m_ueMeasurements.open ("tmp/lena-handover-simple-ue-measurements.dat", std::ofstream::out);
  m_ueMeasurements << "# time   imsi   cellId   isServingCell?  RSRP(dBm)	 RSRQ(dB)" << std::endl;
  m_packetSinkRx.open ("tmp/lena-handover-simple-tcp-receive.dat", std::ofstream::out);
  m_packetSinkRx << "# time   bytesRx" << std::endl;
  m_positionTrace.open ("tmp/lena-handover-simple-position.dat", std::ofstream::out);
  m_positionTrace << "# time    x		y" << std::endl; 
  m_cqiTrace.open ("tmp/lena-handover-simple-cqi.dat", std::ofstream::out);
  m_cqiTrace << "# time   nodeId   rnti  cqi" << std::endl;
  m_tcpCongStateTrace.open ("tmp/lena-handover-simple-tcp-state.dat", std::ofstream::out);
  m_tcpCongStateTrace << "# time    OldState NewState" << std::endl; 	

}

// Function to close custom descriptor files
void
LenaHandoverSimple::CloseFileDescriptors () {

  m_ueMeasurements.close ();
  m_cqiTrace.close ();
  m_packetSinkRx.close ();
  m_tcpCongStateTrace.close ();
  m_positionTrace.close ();

}

// Function to configure default attributes for different components of the NS-3 simulation.
void
LenaHandoverSimple::ConfigureSettings(){

  // Set reasonable default attributes. Reduce number of packets to increase speed.
  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (10)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (1000000));

  // Set to ideal for easier debugging currently.
  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (m_useIdealRrc));

  // Set to false to use PDCCH for SINR calculation.
  Config::SetDefault ("ns3::LteHelper::UsePdschForCqiGeneration", BooleanValue (m_usePdschForSinr));  

  // RLF specific parameters:
  // An out-of-sync indication is received at the UE if the SINR at the UE falls
  // below Qout for NumQoutEvalSf subframes each of length 1 ms. 
  Config::SetDefault ("ns3::LteUePhy::Qout", DoubleValue(m_qOut));
  Config::SetDefault ("ns3::LteUePhy::NumQoutEvalSf", UintegerValue(m_numQoutEvalSf));

  // An in-sync indication is received at the UE if the SINR at the UE rises above 
  // Qin for NumQinEvalSf subframes each of length 1 ms.
  Config::SetDefault ("ns3::LteUePhy::Qin", DoubleValue(m_qIn)); 	
  Config::SetDefault ("ns3::LteUePhy::NumQinEvalSf", UintegerValue(m_numQinEvalSf));

  // An RLF is generated if the T310 timer passes after receiving N310 out-of-sync
  // indications but N311 in-sync indications are not received.
  Config::SetDefault ("ns3::LteUeRrc::T310", TimeValue(MilliSeconds(m_t310)));
  Config::SetDefault ("ns3::LteUeRrc::N310", UintegerValue(m_n310));
  Config::SetDefault ("ns3::LteUeRrc::N311", UintegerValue(m_n311));
	
}

// Function to actually add callbacks based on the code
void
LenaHandoverSimple::ConfigureCallbacks () {

  // Connect custom trace sinks for RRC connection establishment, handover notification
  // and RLF at the UE side.
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
          MakeCallback (&LenaHandoverSimple::NotifyConnectionEstablishedUe, this));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
          MakeCallback (&LenaHandoverSimple::NotifyHandoverStartUe, this));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
          MakeCallback (&LenaHandoverSimple::NotifyHandoverEndOkUe, this));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/RadioLinkFailure",
          MakeCallback (&LenaHandoverSimple::RadioLinkFailure, this));
  Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/RxWithAddresses",
          MakeCallback (&LenaHandoverSimple::NotifyPacketSinkRx, this));

  //Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
  //        MakeCallback (&LenaHandoverSimple::NotifyConnectionEstablishedEnb, this));
  //Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
  //        MakeCallback (&LenaHandoverSimple::NotifyHandoverStartEnb, this));
  //Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
  //        MakeCallback (&LenaHandoverSimple::NotifyHandoverEndOkEnb, this));

  // Additional callbacks related to the experiment
  //Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/LteUePhy/ReportUeMeasurements",
  //        MakeCallback (&LenaHandoverSimple::NotifyUeMeasurements, this));
  //Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/PhySyncDetection",
  //        MakeCallback (&LenaHandoverSimple::PhySyncDetection, this));

  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/ComponentCarrierMap/*/FfMacScheduler/$ns3::RrFfMacScheduler/WidebandCqiReport", MakeCallback (&LenaHandoverSimple::NotifyCqiReport, this));
	
  // Config::ConnectWithoutContext ("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx",
  //         MakeCallback (&LenaHandoverSimple::NotifyPacketSinkRx, this));  	

  // Initiate position tracing for single UE.
  // Simulator::Schedule (Seconds (0), &LenaHandoverSimple::TracePosition, m_ueNodes.Get(0), m_positionTracingInterval);

  // Delay trace connection until TCP socket comes into existence.
  // Simulator::Schedule (Seconds (1.001), &LenaHandoverSimple::ConnectTcpTrace);

  // Additional callbacks for in-sync, out-of-sync detection, UE state transition, RRC messages etc.

  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/StateTransition",
  //        MakeCallback (&LenaHandoverSimple::UeStateTransition, this));
  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteEnbRrc/NotifyConnectionRelease",
  //        MakeCallback (&LenaHandoverSimple::NotifyConnectionReleaseAtEnodeB, this));
  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteEnbRrc/RrcTimeout",
  //        MakeCallback (&LenaHandoverSimple::EnbRrcTimeout, this));
  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/RandomAccessError",
  //        MakeCallback (&LenaHandoverSimple::NotifyRandomAccessErrorUe, this));
  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionTimeout",
  //        MakeCallback (&LenaHandoverSimple::NotifyConnectionTimeoutUe, this));
  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/ComponentCarrierMapUe/*/LteUeMac/RaResponseTimeout",
  //        MakeCallback (&LenaHandoverSimple::NotifyRaResponseTimeoutUe, this));
  // Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/LteEnbRrc/RecvMeasurementReport",
  //        MakeCallback (&LenaHandoverSimple::NotifyRecvMeasurementReport, this));

}

// Setup LTE, start EPC, select HO algo and connect X2 interface.
void
LenaHandoverSimple::SetupLTE () {

  // Setup m_lteHelper and required m_epcHelper.
  m_lteHelper = CreateObject<LteHelper> ();
  m_epcHelper = CreateObject<PointToPointEpcHelper> ();
  m_lteHelper->SetEpcHelper (m_epcHelper);
  m_lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");	

  // Create a single m_remoteHost with a single gateway.
  m_pgw = m_epcHelper->GetPgwNode ();
  m_remoteHostContainer.Create (1);
  m_remoteHost = m_remoteHostContainer.Get (0);

  // Set the HO algorithm.
  if (m_hoAlgo == "A2A4Rsrq") {

    m_lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
    m_lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
	                                        UintegerValue (30));
    m_lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
	                                        UintegerValue (1));
  } else if (m_hoAlgo == "A3Rsrp") {

    m_lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
    m_lteHelper->SetHandoverAlgorithmAttribute ("Hysteresis",
                                                DoubleValue (m_hyst));
    m_lteHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",
                                                TimeValue (MilliSeconds (m_ttt)));
  } else if (m_hoAlgo == "NoOp")
      m_lteHelper->SetHandoverAlgorithmType ("ns3::NoOpHandoverAlgorithm");

  else
    NS_FATAL_ERROR("ERROR! HOAlgo must be set to 'A3Rsrp', 'A2A4Rsq' or 'NoOp'.");

}

// Set the channel model; set to log distance model for enforcing handovers.
void
LenaHandoverSimple::SetChannelModel () {

  m_lteHelper->SetPathlossModelType (TypeId::LookupByName ("ns3::LogDistancePropagationLossModel"));
  m_lteHelper->SetPathlossModelAttribute ("Exponent", DoubleValue (1.0));
  m_lteHelper->SetPathlossModelAttribute ("ReferenceLoss", DoubleValue (38.57)); //ref. loss in dB at 1m for 2.025GHz
  m_lteHelper->SetPathlossModelAttribute ("ReferenceDistance", DoubleValue (1));

  m_lteHelper->SetAttribute ("FadingModel", StringValue ("ns3::TraceFadingLossModel"));
  // script launched by test.py
  m_lteHelper->SetFadingModelAttribute ("TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EVA_60kmph.fad"));
  
}

// Setup remote host, install IP stack and install the UDP protocol.
void
LenaHandoverSimple::SetupInternet () {

  // Connection between EPC and m_internet.
  m_internet.Install (m_remoteHostContainer);

  // Create the m_internet.
  m_p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  m_p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  m_p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  m_internetDevices = m_p2ph.Install (m_pgw, m_remoteHost);
  m_ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  m_internetIpIfaces = m_ipv4h.Assign (m_internetDevices);
  m_remoteHostAddr = m_internetIpIfaces.GetAddress (1);

  // Routing of the m_internet Host (towards the LTE network)
  m_remoteHostStaticRouting = m_ipv4RoutingHelper.GetStaticRouting (m_remoteHost->GetObject<Ipv4> ());
  // interface 0 is localhost, 1 is the p2p device
  m_remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  // Install the IP stack on the UEs.
  m_internet.Install (m_ueNodes);
  m_ueIpIfaces = m_epcHelper->AssignUeIpv4Address (NetDeviceContainer (m_ueLteDevs));
	
  // Setup UDP ports on the UEs, remote host and start applications.
  //	   dlPort ulPort
  InstallUDP (10000, 20000);

  // Initial access for UEs.
  // Use m_lteHelper->Attach (m_ueLteDevs.Get (i), enbLteDevs.Get (0)) for manually attaching all UEs to the first eNodeB. 
  for (uint16_t i = 0; i < m_numberOfUes; i++)
    m_lteHelper->Attach (m_ueLteDevs.Get (i));
    
  // Add X2 interface between eNBs.
  m_lteHelper->AddX2Interface (m_enbNodes);

}

// Set mobility models for both UE and eNBs and install LTE devices on them.
void
LenaHandoverSimple::SetMobility () {

  // Create nodes as specified by the initialization variables.
  m_enbNodes.Create (m_numberOfEnbs);
  m_ueNodes.Create (m_numberOfUes);

  // Install Mobility Model in eNB.
  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < m_numberOfEnbs; i++) {
  
    Vector enbPosition (m_distance * (i + 1), m_distance, 0);
    enbPositionAlloc->Add (enbPosition);

  }
  
  m_enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  m_enbMobility.SetPositionAllocator (enbPositionAlloc);
  m_enbMobility.Install (m_enbNodes);

  // Install Mobility Model in UE.
  m_ueMobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  m_ueMobility.Install (m_ueNodes);
  m_ueNodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, m_yForUe, 0));
  m_ueNodes.Get (0)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (m_speed, 0, 0));

  // Install LTE Devices in eNB and UEs
  m_enbLteDevs = m_lteHelper->InstallEnbDevice (m_enbNodes);
  m_ueLteDevs = m_lteHelper->InstallUeDevice (m_ueNodes);

}

// Install UDP protocol with client/server apps on each UE with bidirectional packet filters.
void
LenaHandoverSimple::InstallUDP (uint16_t dlPort, uint16_t ulPort) {

  for (uint32_t u = 0; u < m_numberOfUes; ++u) {
   
    // Go to each UE.
    Ptr<Node> ue = m_ueNodes.Get (u);
   
    // Set the default gateway for the UE.
    Ptr<Ipv4StaticRouting> ueStaticRouting = m_ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
    ueStaticRouting->SetDefaultRoute (m_epcHelper->GetUeDefaultGatewayAddress (), 1);

    // Add UDP client applications at all UEs.
    for (uint32_t b = 0; b < m_numBearersPerUe; ++b) {
	
      // Increment the port for each UE.
      ++dlPort;
      ++ulPort;
	
      // Client and server apps for each UE
      ApplicationContainer clientApps;
      ApplicationContainer serverApps;

      // Install client/server apps for DL.
      UdpClientHelper dlClientHelper (m_ueIpIfaces.GetAddress (u), dlPort);
      clientApps.Add (dlClientHelper.Install (m_remoteHost));
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",
                                           InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      serverApps.Add (dlPacketSinkHelper.Install (ue));

      // Install client/server apps for UL.
      UdpClientHelper ulClientHelper (m_remoteHostAddr, ulPort);
      clientApps.Add (ulClientHelper.Install (ue));
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                           InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      serverApps.Add (ulPacketSinkHelper.Install (m_remoteHost));

      // EPC traffic flow template with UL/DL traffic filters
      Ptr<EpcTft> tft = Create<EpcTft> ();

      // DL traffic filter with assigned ports
      EpcTft::PacketFilter dlpf;
      dlpf.localPortStart = dlPort;
      dlpf.localPortEnd = dlPort;
      tft->Add (dlpf);

      // UL traffic filter with assigned ports
      EpcTft::PacketFilter ulpf;
      ulpf.remotePortStart = ulPort;
      ulpf.remotePortEnd = ulPort;
      tft->Add (ulpf);

      // Activate EPS bearer with QOS specifications.
      EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
      m_lteHelper->ActivateDedicatedEpsBearer (m_ueLteDevs.Get (u), bearer, tft);

      // Start applications at 0 seconds.	
      serverApps.Start (Seconds (0));
      clientApps.Start (Seconds (0));

    }
  
  }

}

// Function that actually runs the simulation
void
LenaHandoverSimple::Run () {  

  // Set the simulator stopping time.
  Simulator::Stop (Seconds (m_simTime));

  // The RUN command
  Simulator::Run ();

  // Destroy the simulator.
  Simulator::Destroy ();

  // Close any open file descriptors
  CloseFileDescriptors ();

}

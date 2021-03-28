/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

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

#include "lena-handover-simple.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaX2HandoverMeasures");

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

/**
 * Sample simulation script for an automatic X2-based handover based on the RSRP measures for A3 Algo.
 * It instantiates two eNodeB, attaches one UE to the 'source' eNB.
 * The UE moves between both eNBs, it reports measures to the serving eNB and
 * the 'source' (serving) eNB triggers the handover of the UE towards
 * the 'target' eNB when it considers it is a better eNB.
 */
int
main (int argc, char *argv[])
{

  // Default values for parameters from command line
  double simTime = 0.0;                         // [s] calculated based on speed when set to 0.0
  double UEspeed = 20.0;                        // [m/s]
  double enbTxPowerDbm = 43.0;                  // [dBm]
  int num_eNB = 2;                              // number of equally spaced base stations
  int num_Bearers = 0;                          // number of EPS bearers
  float inter_eNB_dist = 500.0;                 // [m]
  float y_for_UE = 1000.0;                      // [m]
  std::string hoAlgo = "A3Rsrp";                // possible values: 'A3Rsrp', 'A2A4Rsrq', 'NoOp'

  // Parameters for A3 HO algo optimization; 3GPP 36.331
  double hysteresis = 3;                        // [dB] standards limit to 0:0.5:15 dB; default to 3 dB; parameter related to RSRP variation after entering A3 condition and before triggering HO
  uint16_t timeToTrigger = 256;                 // [ms] standards limit to {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280}; default to 256 ms; time for which the A3 entering condition must be met to trigger HO

  // Constants specific to RLF detection; 3GPP 36.331
  uint16_t t310 = 50;                           // [ms] standards limit to {0, 50, 100, 200, 500, 1000, 2000}; default to small value of 50 ms but ns-3 defaults to 1000 ms; time after which radio problem detection and no radio recovery to indicate RLF
  uint16_t n310 = 1;                            // standards limit to {1, 2, 3, 4, 6, 8, 10, 20}; default to one out-of-sync indication but ns-3 defaults to 6; N310 out-of-sync indications leads to radio problem detection at UE
  uint16_t n311 = 1;                            // standards limit to {1, 2, 3, 4, 6, 8, 10}; default to one in-sync indication but ns-3 defaults to 2; N311 in-sync indications leads to radio recovery at UE
  bool useIdealRrc = false;                     // use IDEAL RRC based on function calls instead of REAL RRC where RRC messages are passed as ASN encoded packets
  bool usePdschForSinr = false;                 // use PDSCH along with PDCCH for SINR calculation; by default, only PDCCH is used for SINR calculation but ns-3 defaults to using both
  
  // Constants specific to RLF detection; 3GPP 36.133
  uint16_t numQoutEvalSf = 20;                  // [ms] number of subframes of 1 ms each; set to small value for quick detection; numQoutEvalSf is defined as TEval_Qout in #36.133
  uint16_t numQinEvalSf = 10;                   // [ms] number of subframes of 1 ms each; set to small value for quick detection; numQinEvalSf is defined as TEval_Qin in #36.133
  double qOut = -5;                             // [dB] set to ns-3 default of -5 dB; 36.839 sets it to -8 dB
  double qIn = -3.9;                            // [dB] set to ns-3 default of -3.9 dB; 36.839 sets it to -6 dB

  // All UEs follow the same path in this example- so use only 1 UE.
  const int num_UE = 1;

  // Command line arguments
  CommandLine cmd (__FILE__);

  // Specific to simulation
  cmd.AddValue ("simTime", "Total duration of the simulation (in seconds)", simTime);
  cmd.AddValue ("speed", "Speed of the UE (default = 20 m/s)", UEspeed);
  cmd.AddValue ("txPower", "TX power used by HeNBs (default = 43.0 dBm)", enbTxPowerDbm); 
  cmd.AddValue ("neNBs", "Number of eNBs (default = 2)", num_eNB);
  cmd.AddValue ("nBearers", "Number of EPS bearers per UE (default = 0)", num_Bearers);
  cmd.AddValue ("inter_eNB_dist", "Distance between eNBs (default = 500.0 m)", inter_eNB_dist);
  cmd.AddValue ("y_for_UE", "Number of eNBs (default = 1000.0 m)", y_for_UE);

  // Parameters for HO algorithm; 3GPP 36.331
  cmd.AddValue ("hoAlgo", "HO algorithm used (default = 'A3Rsrp'); possible values include: 'A3Rsrp', 'A2A4Rsrq', 'NoOp'", hoAlgo);
  cmd.AddValue ("hysteresis", "Hysteresis value (default = 3 dB); standard (#36.331) limits to {0, 0.5, ... 15}", hysteresis);
  cmd.AddValue ("timeToTrigger", "Time to trigger (TTT) (default = 256 ms); standard (#36.331) limits to {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280}", timeToTrigger);

  // Constants for RLF detection; 3GPP 36.133
  cmd.AddValue ("qOut", "Threshold on SINR for out-of-sync indications (default = -5 dB); any possible value but set to -8 dB in #36.839 and ns-3 defaults to -5 dB", qOut);
  cmd.AddValue ("qIn", "Threshold on SINR for in-of-sync indications (default = -3.9 dB); any possible value but set to -6 dB in #36.839 and ns-3 defaults to -3.9 dB", qIn);
  cmd.AddValue ("numQoutEvalSf", "Number of subframes (1 ms each) before out-of-sync notification sent to eNb (default = 20); any possible +ve integer but ns-3 defaults to 200", numQoutEvalSf);
  cmd.AddValue ("numQinEvalSf", "Number of subframes (1 ms each) before in-of-sync notification sent to eNb (default = 10); any possible +ve integer but ns-3 defaults to 100", numQinEvalSf);
  
  // Constants for RLF detection; 3GPP 36.331
  cmd.AddValue ("t310", "Timer after radio problem detection and no recovery for RLF indication (default = 50 ms); standard (#36.331) limits to {0, 50, 100, 200, 500, 1000, 2000} but ns-3 defaults to 1000 ms", t310);
  cmd.AddValue ("n310", "Number of out-of-sync notifications for radio problem detection at UE (default = 1); standard (#36.331) limits to {1, 2, 3, 4, 6, 8, 10, 20} but ns-3 defaults to 6", n310);
  cmd.AddValue ("n311", "Number of in-sync notifications for radio recovery at UE (default = 1); standard (#36.331) limits to {1, 2, 3, 4, 5, 6, 8, 10} but ns-3 defaults to 2", n311);  

  // NS-3 specific parameters
  cmd.AddValue ("usePdschForSinr", "Use PDSCH along with PDCCH for SINR calculation; by default, only PDCCH is used for SINR calculation but ns-3 defaults to using both", usePdschForSinr);
  cmd.AddValue ("useIdealRrc", "Use IDEAL RRC based on function calls instead of REAL RRC where RRC messages are passed as ASN encoded packets; ns-3 defaults to ", useIdealRrc);  

  cmd.Parse (argc, argv);

  // Simulation parameters:      num_UE  num_eNB  num_Bearers  inter_eNB_dist  y_for_UE  UESpeed     eNBTXPower  simTime 
  LenaHandoverSimple simpleTest (num_UE, num_eNB, num_Bearers, inter_eNB_dist, y_for_UE, UEspeed, enbTxPowerDbm, simTime,
  // RLF parameters:            qOut  qIn  numQoutEvalSf  numQinEvalSf  t310  n310  n311  useIdealRrc  usePdschForSinr
                                qOut, qIn, numQoutEvalSf, numQinEvalSf, t310, n310, n311, useIdealRrc, usePdschForSinr,
  // HO optimization parameters:hoAlgo  hysteresis  timeToTrigger
                                hoAlgo, hysteresis, timeToTrigger);
  
  // Run the object to get the logs as the measurements.
  simpleTest.Run ();

  return 0;

}

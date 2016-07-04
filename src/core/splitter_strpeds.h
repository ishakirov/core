//
//  splitter_strpeds.h
//  P2PSP
//
//  This code is distributed under the GNU General Public License (see
//  THE_GENERAL_GNU_PUBLIC_LICENSE.txt for extending this information).
//  Copyright (C) 2016, the P2PSP team.
//  http://www.p2psp.org
//

#ifndef P2PSP_CORE_SPLITTER_STRPEDS_H_
#define P2PSP_CORE_SPLITTER_STRPEDS_H_

#include <stdio.h>
#include <stdlib.h>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/tokenizer.hpp>

#include <map>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <exception>

#include "../util/trace.h"
#include "splitter_dbs.h"
#include "common.h"
#include "openssl/dsa.h"

namespace p2psp {

class null: public std::exception {
 public:
  null() {}
  ~null() {}
};

class SplitterSTRPEDS : public SplitterDBS {
 protected:
  const int kDigestSize = 40;
  const int kGatherBadPeersSleep = 5;
  const bool kLogging = false;
  const int kCurrentRound = 0;

  int p_mpl_ = 100;
  int p_tpl_ = 100;

  int digest_size_;
  int gather_bad_peers_sleep_;
  bool logging_;
  std::ofstream log_file_;
  std::ifstream trusted_file_;
  int current_round_;

  std::vector<boost::asio::ip::udp::endpoint> trusted_peers_;
  std::vector<boost::asio::ip::udp::endpoint> trusted_peers_discovered_;
  std::vector<boost::asio::ip::udp::endpoint> bad_peers_;

	// P2PSP TMS
	double maxTrust = 2.;
	double incr = .05;
	double decr = .05;
	std::map<boost::asio::ip::udp::endpoint, int> peer_lifetimes_;
	std::map<boost::asio::ip::udp::endpoint, std::set<boost::asio::ip::udp::endpoint> > peer_unique_complains_;
	std::map<boost::asio::ip::udp::endpoint, double> peer_penalties_;

  int gathering_counter_;
  int trusted_gathering_counter_;
  std::vector<boost::asio::ip::udp::endpoint> gathered_bad_peers_;
  std::vector<std::vector<boost::asio::ip::udp::endpoint> > complains_;
  int majority_ratio_;

  DSA* dsa_key;

  // endpoint -> position in the complains_ vector.
  std::map<boost::asio::ip::udp::endpoint, int> complains_map_;

  // Thread management
  void Run() override;


 public:
  SplitterSTRPEDS();
  ~SplitterSTRPEDS();
  void SetMajorityRatio(int majority_ratio);
  void HandleAPeerArrival(
      std::shared_ptr<boost::asio::ip::tcp::socket> serve_socket) override;
  void SendDsaKey(const std::shared_ptr<boost::asio::ip::tcp::socket> &sock);
  void GatherBadPeers();
  boost::asio::ip::udp::endpoint GetPeerForGathering();
  boost::asio::ip::udp::endpoint GetTrustedPeerForGathering();
  void RequestBadPeers(const boost::asio::ip::udp::endpoint &dest);
  void InitKey();
  std::vector<char> GetMessage(int chunk_number, const boost::asio::streambuf &chunk, const boost::asio::ip::udp::endpoint &dst);

  void AddTrustedPeer(const boost::asio::ip::udp::endpoint &peer);

  void ModerateTheTeam();
  void ProcessBadPeersMessage(const std::vector<char> &message, const boost::asio::ip::udp::endpoint &sender);
  void HandleBadPeerFromTrusted(const boost::asio::ip::udp::endpoint &bad_peer, const boost::asio::ip::udp::endpoint &sender);
  void HandleBadPeerFromRegular(const boost::asio::ip::udp::endpoint &bad_peer, const boost::asio::ip::udp::endpoint &sender);
  void AddComplain(const boost::asio::ip::udp::endpoint &bad_peer, const boost::asio::ip::udp::endpoint &sender);
  void PunishPeer(const boost::asio::ip::udp::endpoint &bad_peer, std::string message);
  void OnRoundBeginning();
  void RefreshTPs();
  void PunishPeers();
  void PunishTPs();

  void SetLogging(bool enabled);
  void SetPMPL(int probability);
  int GetPMPL();
  void SetPTPL(int probability);
  int GetPTPL();
  void SetLogFile(const std::string &filename);
  void LogMessage(const std::string &message);
  std::string BuildLogMessage(const std::string &message);

  void IncrementUnsupportivityOfPeer(const boost::asio::ip::udp::endpoint &peer) override;

  // Thread management
  void Start();

	double ComputePeerTrustValue(const boost::asio::ip::udp::endpoint &peer);
	void RunTMS();
	void SetMaxTrust(double maxTrust);
	double GetMaxTrust();
	void SetIncr(double incr);
	double GetIncr();
	void SetDecr(double decr);
	double GetDecr();
};
}  // namespace p2psp

#endif  // P2PSP_CORE_SPLITTER_STRPEDS_H_

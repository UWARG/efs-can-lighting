/*
 * can_state.hpp
 *
 * Created on: Jan 26, 2025
 *     Author: Nick Armstrong
 */

#ifndef INC_CAN_STATE_HPP_
#define INC_CAN_STATE_HPP_

struct Packet {
  PacketKind kind;
  uint32_t id;
};

enum PacketKind {
  ARDUPILOT_PACKET, INTERNAL_PACKET, MASTER_DEATH_PACKET, START_ELECTION_PACKET, ASSERT_MASTER_PACKET
};

class NodeState {
  NodeStateTag tag;
  NodeStateValue value;

  // Tell the state machien that a packet was recieved.
  void handlePacket(Packet packet, uint32_t time);
  // Update the state machine. should be called at min 10 Hz.
  void update(PacketKind &toSend, uint32_t time);
};

enum NodeStateTag {
  NODE_MASTER_STATE, NODE_FOLLOWER_STATE, NODE_CANDIDATE_STATE
};

union NodeStateValue {
  NodeMasterState master;
  NodeFollowerState follower;
  NodeCandidateState candidate;
};

class NodeMasterState {
public:
  NodeState(uint32_t id, uint32_t last_ardupilot_packet);
  void handlePacket(Packet packet, uint32_t time);
private:
  const uint32_t id;
  // if the time since last ardupilot packet exceeds `MAX_PACKET_DELAY`, this node gives up its role as master, broadcasting a `MASTER_DEATH` packet
  uint32_t last_ardupilot_packet;
  static constexpr uint32_t MAX_PACKET_DELAY = 5000;
};

class NodeFollowerState {
public:
  NodeFollowerState(uint32_t id, uint32_t last_master_packet, uint32_t last_ardupilot_packet);
  void handlePacket(Packet packet, uint32_t time) override;
private:
  const uint32_t id;
  // if the time since last master packet exceeds `MAX_PACKET_DELAY`, and the time since it last saw an ardupilot packet is less than `NodeMasterState.MAX_PACKET_DELAY` this node transitions to `NodeCandidateState` where it either transitions back to a `NodeFollowerState` or `NodeMasterState`
  // if the node receives a `NodeMasterState`, it transitions to a `NodeCandidateState`
  uint32_t last_master_packet;
  uint32_t last_ardupilot_packet;
  bool should_transition = false;
  static constexpr uint32_t MAX_PACKET_DELAY = 100;
};

// A `NodeCandidateState` sends out an `START_ELECTION` packet. If after `ELECTION_DURATION` ticks the candidate hasn't received an `ELECTION_DURATION` from a node with a lower ID, it broadcasts a `ASSERT_MASTER` message and transitions to a `NodeMasterState`.
class NodeCandidateState {
public:
  NodeCandidateState(uint32_t id, uint32_t election_start);
  void handlePacket(Packet packet, uint32_t time);
private:
  const uint32_t id;
  uint32_t election_start;
  bool election_failed = false;
  static constexpr uint32_t ELECTION_DURATION = 100;
};

#endif

/*
 * can_state.hpp
 *
 * Created on: Jan 26, 2025
 *     Author: Nick Armstrong
 */

#ifndef INC_CAN_STATE_HPP_
#define INC_CAN_STATE_HPP_

#include <cstdint>

enum PacketKind {
  ARDUPILOT_PACKET, INTERNAL_PACKET, ASSERT_MASTER_PACKET
};

struct Packet {
  PacketKind kind;
  uint32_t id;
};

constexpr uint32_t MAX_ARDUPILOT_PACKET_DELAY = 5000;
constexpr uint32_t MAX_MASTER_PACKET_DELAY = 100;

class NodeState;

// if the time since last ardupilot packet exceeds `MAX_ARDUPILOT_PACKET_DELAY`, the node gives up its status as master
// additionally, if it recieves an ASSERT_MASTER packet from a node with a lower ID, it gives up its status as master
class NodeMasterState {
public:
  NodeMasterState(uint32_t id, uint32_t last_ardupilot_packet);
  void handlePacket(Packet packet, uint32_t time);
  bool update(NodeState &state, PacketKind &toSend, uint32_t time);
private:
  uint32_t id;
  uint32_t last_ardupilot_packet;
  bool should_transition = false;
};

// if the time since last master packet exceeds `MAX_MASTER_PACKET_DELAY` and an ardupilot packet has been seen recently, it sends an ASSERT_MASTER packet and becomes a master
class NodeFollowerState {
public:
  NodeFollowerState(uint32_t id, uint32_t last_master_packet, uint32_t last_ardupilot_packet);
  void handlePacket(Packet packet, uint32_t time);
  bool update(NodeState &state, PacketKind &toSend, uint32_t time);
private:
  uint32_t id;
  uint32_t last_master_packet;
  uint32_t last_ardupilot_packet;
  bool should_transition = false;
};

enum NodeStateTag {
  NODE_MASTER_STATE, NODE_FOLLOWER_STATE
};

union NodeStateValue {
  NodeMasterState master;
  NodeFollowerState follower;
};

class NodeState {
public:
  NodeState(uint32_t id, uint32_t time);
  // Tell the state machien that a packet was recieved.
  void handlePacket(Packet packet, uint32_t time);
  // Update the state machine. should be called at min 10 Hz. `toSend` is an out param that is valid if `update` returns true.
  bool update(PacketKind &toSend, uint32_t time);
  void transition(NodeMasterState state);
  void transition(NodeFollowerState state);
private:
  NodeStateTag tag;
  NodeStateValue value;  
};

#endif

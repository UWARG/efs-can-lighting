/*
 * can_state.hpp
 *
 * Created on: Jan 26, 2025
 *     Author: Nick Armstrong
 */

#ifndef INC_CAN_STATE_HPP_
#define INC_CAN_STATE_HPP_



#include <variant>

class NodeState {
private:
  NodeStateValue value;
  NodeStateTag tag;
};

union NodeStateValue {
  NodeMasterState master;
  NodeFollowerState follower;
};

enum NodeStateTag {
  NODE_MASTER_STATE, NODE_FOLLOWER_STATE
};

class NodeMasterState {
private:
  // if the time since last ardupilot packet exceeds MAX_PACKET_DELAY, this node gives up its role as master, broadcasting an appropriate message
  uint32_t last_ardupilot_packet;
  static constexpr uint32_t MAX_PACKET_DELAY;
};

class NodeFollowerState {
private:
  // if the time since last master packet exceeds MAX_PACKET_DELAY, this node transitions to `NodeProspectiveState` where it either transitions back to a `NodeFollowerState` or `NodeMasterState`
  // if the node receives a `NodeMasterState`, it transitions to a `NodeProspectiveState`
  uint32_t last_master_packet;
  static constexpr uint32_t MAX_PACKET_DELAY;
};

class NodeProspectiveState {
private:
};

#endif

/*
 * can_state.hpp
 *
 * Created on: Jan 26, 2025
 *     Author: Nick Armstrong
 */

#ifndef INC_CAN_STATE_HPP_
#define INC_CAN_STATE_HPP_

class NodeState {
private:
  NodeStateValue value;
  NodeStateTag tag;
};

union NodeStateValue {
  NodeMasterState master;
  NodeFollowerState follower;
  NodeCandidateState candidate;
};

enum NodeStateTag {
  NODE_MASTER_STATE, NODE_FOLLOWER_STATE, NODE_CANDIDATE_STATE
};

class NodeMasterState {
private:
  // if the time since last ardupilot packet exceeds `MAX_PACKET_DELAY`, this node gives up its role as master, broadcasting a `MASTER_DEATH` packet
  uint32_t last_ardupilot_packet;
  static constexpr uint32_t MAX_PACKET_DELAY;
};

class NodeFollowerState {
private:
  // if the time since last master packet exceeds `MAX_PACKET_DELAY`, this node transitions to `NodeCandidateState` where it either transitions back to a `NodeFollowerState` or `NodeMasterState`
  // if the node receives a `NodeMasterState`, it transitions to a `NodeCandidateState`
  uint32_t last_master_packet;
  static constexpr uint32_t MAX_PACKET_DELAY;
};

// A `NodeCandidateState` sends out an `START_ELECTION` packet. If after `ELECTION_DURATION` ticks the candidate hasn't received an `ELECTION_DURATION` from a node with a lower ID, it broadcasts a `ASSERT_MASTER` message and transitions to a `NodeMasterState`.
class NodeCandidateState {
private:
  uint32_t election_start;
  static constexpr uint32_t ELECTION_DURATION;
};

#endif

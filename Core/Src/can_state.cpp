/*
 * can_state.cpp
 *
 * Created on: Jan 26, 2025
 *     Author: Nick Armstrong
 */

#include "can_state.hpp"

NodeState::NodeState(uint32_t id, uint32_t time): tag(NODE_FOLLOWER_STATE) {
  value.follower = NodeFollowerState(id, time, time);
}
NodeState::handlePacket(Packet packet, uint32_t time) {
  switch (tag) {
    case NODE_MASTER_STATE: {
      value.master.handlePacket(packet, time);
      break;
    }
    case NODE_FOLLOWER_STATE: {
      value.follower.handlePacket(packet, time);
      break;
    }
  }
}
bool NodeState::update(PacketKind &toSend, uint32_t time) {
  switch (tag) {
    case NODE_MASTER_STATE: {
      value.master.update(this, packet, time);
      break;
    }
    case NODE_FOLLOWER_STATE: {
      value.follower.update(this, packet, time);
      break;
    }
  }
}
void NodeState::transition(NodeMasterState state) {
  tag = NODE_MASTER_STATE;
  tag.value.master = state;
}
void NodeState::transition(NodeFollowerState state) {
  tag = NODE_FOLLOWER_STATE;
  tag.value.follower = state;
}

NodeMasterState::NodeMasterState(uint32_t id, uint32_t last_ardupilot_packet): id(id), last_ardupilot_packet(last_ardupilot_packet) {}
void NodeMasterState::handlePacket(Packet packet, uint32_t time) override {
  if (packet.kind == ARDUPILOT_PACKET) {
    last_ardupilot_packet = time;
  } else if (packet.kind == ASSERT_MASTER && id > packet.id) {
    should_transition = true;
  }
}
bool NodeMasterState::update(NodeState &state, PacketKind &toSend, uint32_t time) {
  if (should_transition || last_ardupilot_packet + MAX_ARDUPILOT_PACKET_DELAY < time) {
    state.transition(NodeFollowerState(id, time, last_ardupilot_package));
    return true;
  } else {
    return false;
  }
}

NodeFollowerState::NodeFollowerState(uint32_t id, uint32_t last_master_packet, uint32_t last_ardupilot_packet): id(id), last_master_packet(last_master_packet), last_ardupilot_packet(last_ardupilot_packet) {}
void NodeFollowerState::handlePacket(Packet packet, uint32_t time) override {
  if (packet.kind == ARDUPILOT_PACKET) {
    last_ardupilot_packet = time;
  } else if (packet.kind == INTERNAL_PACKET || packet.kind == ASSERT_MASTER) {
    last_master_packet = time;
  } else if (last_master_packet + MAX_MASTER_PACKET_DELAY < time && last_ardupilot_packet + MAX_ARDUPILOT_PACKET_DELAY > time) {
    should_transition = true;
  }
}
bool NodeFollowerState::update(NodeState &state, PacketKind &toSend, uint32_t time) {
  if (should_transition) {
    state.transition(NodeCandidateState(id, time));
    toSend = ASSERT_MASTER;
    return true;
  } else {
    return false;
  }
}

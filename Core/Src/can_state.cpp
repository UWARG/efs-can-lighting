/*
 * can_state.cpp
 *
 * Created on: Jan 26, 2025
 *     Author: Nick Armstrong
 */

#include "can_state.hpp"

NodeFollowerState::NodeFollowerState(uint32_t id, uint32_t last_ardupilot_packet): id(id), last_ardupilot_packet(last_ardupilot_packet) {}
void NodeMasterState::handlePacket(Packet packet, uint32_t time) override {
  if (packet.kind == ARDUPILOT_PACKET) {
    last_ardupilot_packet = time;
  }
}

NodeFollowerState::NodeFollowerState(uint32_t id, uint32_t last_master_packet, uint32_t last_ardupilot_packet): id(id), last_master_packet(last_master_packet), last_ardupilot_packet(last_ardupilot_packet) {}
void NodeFollowerState::handlePacket(Packet packet, uint32_t time) override {
  if (packet.kind == ARDUPILOT_PACKET) {
    last_ardupilot_packet = time;
  } else if (packet.kind == INTERNAL_PACKET) {
    last_master_packet = time;
  } else if (packet.kind == MASTER_DEATH_PACKET || (packet.kind == START_ELECTION_PACKET && packet.id > id)) {
    should_transition = true;
  }
}

NodeCandidateState::NodeCandidateState(uint32_t id, uint32_t election_start): id(id), election_start(election_start) {}
void NodeCandidateState::handlePacket(Packet packet, uint32_t time) override {
  if (packet.kind == ASSERT_MASTER || (packet.kind == START_ELECTION_PACKET && packet.id < id)) {
    election_failed = true;
  }
}

NodeState::NodeState(uint32_t id, uint32_t time) {
  tag = NODE_CANDIDATE_STATE;
  value = NodeFollowerState(id, time, time);
}

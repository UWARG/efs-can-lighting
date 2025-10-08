/*
 * dronecan_node.c
 *
 *  Created on: Oct 7, 2025
 *      Author: micah
 */

#include "canard.h"
#include "canard_stm32_driver.h"
#include "dronecan_msgs.h"
#include "uavcan.protocol.GetNodeInfo_res.h"


CanardInstance canard;
uint8_t memory_pool[1024];

struct uavcan_protocol_NodeStatus node_status;

/*
  handle a GetNodeInfo request
*/
void handle_GetNodeInfo(CanardInstance *ins, CanardRxTransfer *transfer)
{
    uint8_t buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];
    struct uavcan_protocol_GetNodeInfoResponse pkt;

    memset(&pkt, 0, sizeof(pkt));

    node_status.uptime_sec = micros64() / 1000000ULL;
    pkt.status = node_status;

    // fill in your major and minor firmware version
    pkt.software_version.major = 1;
    pkt.software_version.minor = 2;
    pkt.software_version.optional_field_flags = 0;
    pkt.software_version.vcs_commit = 0; // should put git hash in here

    // should fill in hardware version
    pkt.hardware_version.major = 2;
    pkt.hardware_version.minor = 3;

    getUniqueID(pkt.hardware_version.unique_id);

    strncpy((char*)pkt.name.data, "SimpleNode", sizeof(pkt.name.data));
    pkt.name.len = strnlen((char*)pkt.name.data, sizeof(pkt.name.data));

    uint16_t total_size = uavcan_protocol_GetNodeInfoResponse_encode(&pkt, buffer);

    canardRequestOrRespond(ins,
                           transfer->source_node_id,
                           UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE,
                           UAVCAN_PROTOCOL_GETNODEINFO_ID,
                           &transfer->transfer_id,
                           transfer->priority,
                           CanardResponse,
                           &buffer[0],
                           total_size);
}


void dronecan_on_can_rx(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    uint8_t data[8];
    CanardCANFrame frame;

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, data) != HAL_OK)
        return;

    frame.id = (rx_header.IDE == CAN_ID_EXT) ? rx_header.ExtId : rx_header.StdId;
    if (rx_header.IDE == CAN_ID_EXT)
        frame.id |= CANARD_CAN_FRAME_EFF;
    frame.data_len = rx_header.DLC;
    memcpy(frame.data, data, rx_header.DLC);
    canardHandleRxFrame(&canard, &frame, micros64());
}

/*
 This callback is invoked by the library when a new message or request or response is received.
*/
void onTransferReceived(CanardInstance *ins, CanardRxTransfer *transfer)
{
    // switch on data type ID to pass to the right handler function
    if (transfer->transfer_type == CanardTransferTypeRequest) {
        // check if we want to handle a specific service request
        switch (transfer->data_type_id) {
        case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
            handle_GetNodeInfo(ins, transfer);
            break;
        }
        }
    }
}


/*
 This callback is invoked by the library when it detects beginning of a new transfer on the bus that can be received
 by the local node.
 If the callback returns true, the library will receive the transfer.
 If the callback returns false, the library will ignore the transfer.
 All transfers that are addressed to other nodes are always ignored.

 This function must fill in the out_data_type_signature to be the signature of the message.
 */
bool shouldAcceptTransfer(const CanardInstance *ins,
                                 uint64_t *out_data_type_signature,
                                 uint16_t data_type_id,
                                 CanardTransferType transfer_type,
                                 uint8_t source_node_id)
{
    if (transfer_type == CanardTransferTypeRequest) {
        // check if we want to handle a specific service request
        switch (data_type_id) {
        case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
            *out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE;
            return true;
        }
        }
    }
    // we don't want any other messages
    return false;
}


/*
  send the 1Hz NodeStatus message. This is what allows a node to show
  up in the DroneCAN GUI tool and in the flight controller logs
 */
void send_NodeStatus(void)
{
    uint8_t buffer[UAVCAN_PROTOCOL_NODESTATUS_MAX_SIZE];

    node_status.uptime_sec = micros64() / 1000000ULL;
    node_status.health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
    node_status.mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL;
    node_status.sub_mode = 0;
    // put whatever you like in here for display in GUI
    node_status.vendor_specific_status_code = 1234;

    uint32_t len = uavcan_protocol_NodeStatus_encode(&node_status, buffer);

    // we need a static variable for the transfer ID. This is
    // incremeneted on each transfer, allowing for detection of packet
    // loss
    uint8_t transfer_id;

    canardBroadcast(&canard,
                    UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                    UAVCAN_PROTOCOL_NODESTATUS_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    buffer,
                    len);
}

/*
  This function is called at 1 Hz rate from the main loop.
*/
void process1HzTasks(uint64_t timestamp_usec)
{
    /*
      Purge transfers that are no longer transmitted. This can free up some memory
    */
    canardCleanupStaleTransfers(&canard, timestamp_usec);

    /*
     Transmit the node status message
     */
    send_NodeStatus();
}

void init(uint8_t node_id) {
    canardInit(&canard,
               memory_pool,
               sizeof(memory_pool),
               onTransferReceived,
               shouldAcceptTransfer,
               NULL);
    canardSetLocalNodeID(&canard, node_id);
}

/*
  Transmits all frames from the TX queue, receives up to one frame.
*/
void processTxRxOnce(CAN_HandleTypeDef *hcan)
{
    // Transmitting
    for (const CanardCANFrame* txf = NULL; (txf = canardPeekTxQueue(&canard)) != NULL;) {
        const int16_t tx_res = canardSTM32Transmit(hcan, txf);
        if (tx_res < 0) {         // Failure - drop the frame
            canardPopTxQueue(&canard);
        }
        else if (tx_res > 0)    // Success - just drop the frame
        {
            canardPopTxQueue(&canard);
        }
        else                    // Timeout - just exit and try again later
        {
            break;
        }
    }

    // Receiving
    CanardCANFrame rx_frame;
    const int16_t rx_res = canardSTM32Recieve(&canard, CAN_RX_FIFO0, &rx_frame);
    if (rx_res < 0) {
        ;
    }
    else if (rx_res > 0)        // Success - process the frame
    {
        canardHandleRxFrame(&canard, &rx_frame, micros64());
    }
}

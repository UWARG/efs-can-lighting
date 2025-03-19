-- enabling CAN in missin planner
-- 1: CONFIG > Full Parameter LIst
-- CAN_P1_DRIVER = 1
-- CAN_P1_BITRATE = 1000000
-- CAN_P1_DEBUG = 2
-- CAN_P2_DRIVER = 1
-- CAN_P2_BITRATE = 1000000
-- CAN_P2_DEBUG = 2
-- CAN_D1_PROTOCOL = 1
-- Rebout pixhawk
-- run can.get_dev(0)

local states = {
    startup = 0,
    ground = 1,
    taxi = 2,
    takeoff = 3,
    flight = 4,
    landing =5
}
local drone_state = states.startup --Initialize drone state

local device = CAN:get_device(20) --get a can bus device handler
if device == nil then --sends error message if device not found
    gcs:send_text(3, "CAN device not found!")
    return
end

local can_message_ID = uint32_t(10) --sets CAN ID
local can_bus = uint32_t(1) -- CAN bus to use

local function get_control_state() --function to get the current control state of the drone
    if arming:arm() == true and arming:is_armed() == false then --check if drone is ready to arm bus isn't
        drone_state = states.ground
    elseif arming:is_armed() == true and motors:get_throttle() == 0 then --check if drone is armed but throttle is 0
        drone_state = states.taxi
    elseif motors:get_throttle() ~= 0 and ahrs:altitude() < 5 then --check if drone is below 5m but throttle not 0
        drone_state = states.takeoff
    elseif ahrs:altitude() >= 5 and arming:is_armed() == true then --check if drone above 5m
        drone_state = states.flight
    elseif vehicle:is_landing() == true then --check if drone is landing
        drone_state = states.landing
    else 
        drone_state = states.startup
    end

    local message = CANFrame() --Create new CAN frame
    message:id(can_message_ID) -- Set CAN message ID
    message:data(0, drone_state) -- Set data for drone state

    if can.send({id = message:id(), ext = false, data = {message:data(0)}}) then --Send CAN frame
        gcs:send_text(6, string.format("Drone state: %d", drone_state)) -- display drone state
    else
        gcs:send_text(3, "CAN Send Failed!")
    end
    return get_control_state(), 100
        
end
return get_control_state()


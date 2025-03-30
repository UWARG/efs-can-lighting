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
local message_counter = 0;
local device = CAN:get_device(20) --get a can bus device handler
if device == nil then --sends error message if device not found
    gcs:send_text(3, "Lua: CAN device not found!")
end

local function get_control_state() --function to get the current control state of the drone
    local location = ahrs:get_location()
    if arming:arm() == true and arming:is_armed() == false then --check if drone is ready to arm bus isn't
        drone_state = states.ground
    elseif arming:is_armed() == true and motors:get_throttle() == 0 then --check if drone is armed but throttle is 0
        drone_state = states.taxi
    elseif motors:get_throttle() ~= 0 and ahrs:altitude() < 5 then --check if drone is below 5m but throttle not 0
        drone_state = states.takeoff
    elseif location ~= nil and location:alt() >= 500 and arming:is_armed() == true then --check if drone above 5m
        drone_state = states.flight
    elseif vehicle:is_landing() == true then --check if drone is landing
        drone_state = states.landing
    else 
        drone_state = states.startup
    end

    local message = CANFrame() --Create new CAN frame
    message:id(0x18015510) -- Set CAN message ID
    message:dlc(2)
    message:data(0, drone_state)
    message:data(1, message_counter) -- Set data for drone state
    message_counter = (message_counter + 1) % 256

    device:write_frame(message, 1000);
    gcs:send_text(3, 
        "Lua: sent frame"
    )

    return get_control_state, 100
        
end
return get_control_state()
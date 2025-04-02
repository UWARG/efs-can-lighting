local states = {
    startup = 8,
    ground = 0,
    taxi = 2,
    takeoff = 3,
    flight = 4,
    landing = 6
}

local state_list = {states.startup, states.ground, states.taxi, states.takeoff, states.flight, states.landing}

local transfer_id = 0;
local device = CAN:get_device(5) --get a can bus device handler
if device == nil then --sends error message if device not found
    gcs:send_text(3, "Lua: CAN device not found!")
    return
end

local function make_id(priority, message_id, node_id)
    local proper_id = uint32_t(priority % 32) << 24 | uint32_t(message_id % 65536) << 8 | uint32_t(node_id % 128)
    gcs:send_text(3, "Lua: computed proper id of " .. tostring(proper_id))
    local total_id = uint32_t(1) << 31 | proper_id
    gcs:send_text(3, "Computed total id of " .. tostring(total_id))
    return total_id
end

local function make_tail_byte(start, end_, toggle, id)
    byte = id % 32
    if start then byte = byte | 128 end
    if end_ then byte = byte | 64 end
    if toggle then byte = byte | 32 end
    return byte
end

local function make_single_frame_tail_byte(id)
    return make_tail_byte(true, true, false, id)
end

local state_index = 1


--local function calculate_current_state()
--    state_index = state_index + 1
--    if state_index > 6 then state_index = 1 end
--    local state_name = state_list[state_index]
--
--    return state_name
--end


local function calculate_current_state()
    local location = ahrs:get_location()
    if arming:pre_arm_checks() == false and arming:is_armed() == false then --check if drone is ready to arm bus isn't
        return states.startup
    elseif arming:pre_arm_checks() and arming:is_armed() == false then
        return states.ground
    elseif arming:is_armed() == true and motors:get_throttle() == 0 then --check if drone is armed but throttle is 0
        return states.taxi
    elseif motors:get_throttle() ~= 0 and location ~= nil and location:alt() < 500 then --check if drone is below 5m but throttle not 0
        return states.takeoff
    elseif location ~= nil and location:alt() >= 500 and arming:is_armed() == true then --check if drone above 5m
        return states.flight
    elseif vehicle:is_landing() == true then --check if drone is landing
        return states.landing
    else 
        return states.startup
    end
end

local function get_control_state() --function to get the current control state of the drone
    local state = calculate_current_state()
    local message = CANFrame() --Create new CAN frame
    local id = make_id(24, 800, 11)
    gcs:send_text(3, "found id " .. tostring(id))
    message:id(id)
    message:dlc(2)
    message:data(0, state)
    message:data(1, make_single_frame_tail_byte(transfer_id))

    transfer_id = transfer_id + 1
    if transfer_id > 31 then transfer_id = 0 end

    if device:write_frame(message, 10000) then
        gcs:send_text(3, 
            "Lua: sent frame with ID " .. tostring(id) .. " and state name " .. state
        )
    else
        gcs:send_text(3, "Lua: failed to send frame with ID " .. tostring(id))
    end

    return get_control_state, 5000
        
end
return get_control_state()
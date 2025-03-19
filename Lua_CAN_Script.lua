-- enabling CAN in missin planner
-- 1: CONFIG > Full Parameter LIst
-- CAN_P1_DRIVER = 1
-- CAN_P1_BITRATE = 1000000
-- CAN_P1_DEBUG = 2
-- CAN_P2_DRIVER = 1
-- CAN_P2_BITRATE = 1000000
-- CAN_P2_DEBUG = 2
-- CAN_D1_PROTOCOL = 1
-- reboot Pixhawk
-- run can.get_dev(0)

local can_dev = can.get_dev(0)

local function send_flight_data() --function to get flight data from pixhawk microcontroller
    local altitude = ahrs:get_altitude() or 0 --if true, returns altitude value, if nil, returns 0 (m)
    local speed = ahrs:get_speed() or 0 -- returns aircraft speed (m/s)
    local pitch = ahrs:get_pitch() or 0 --returns pitch heading in radians
    local yaw = ahrs:get_yaw() or 0 --returns yaw heading in radians

    local alt_int = math.floor(altitude *100) --converting all values to integers, easier to send 
    local spd_int = math.floor(speed * 100)
    local pit_int = math.floor(pitch * 100)
    local yaw_int = math.floor(yaw * 100)

    local flight_data_frame_1 = {
        (alt_int >> 24) & 0xFF,  -- Converting 32 bit integers into 4 8-bit byte to sent over frame, split into two 8 byte CAN frames
        (alt_int >> 16) & 0xFF,
        (alt_int >> 8)  & 0xFF,
        alt_int & 0xFF,          

        (spd_int >> 24) & 0xFF,
        (spd_int >> 16) & 0xFF,
        (spd_int >> 8)  & 0xFF,
        spd_int & 0xFF,
    }

    local flight_data_frame_2 = {
        (pit_int >> 24) & 0xFF,
        (pit_int >> 16) & 0xFF,
        (pit_int >> 8)  & 0xFF,
        pit_int & 0xFF,
    
        (yaw_int >> 24) & 0xFF,
        (yaw_int >> 16) & 0xFF,
        (yaw_int >> 8)  & 0xFF,
        yaw_int & 0xFF,    
    }
    
    -- send CAN frame 1 (ID: 0x200)
    local can_id1 = 0x200 
    local success1 = can_dev:send(can_id1, false, flight_data_frame_1)

    -- send CAN frame 2 (ID: 0x201)
    local can_id2 = 0x201 
    local success2 = can_dev:send(can_id2, false, flight_data_frame_2)

    -- Debug output, reports message if 
    if success1 and success2 then
        gcs:send_text(6, string.format("CAN Sent: Alt=%d cm, Vel=%d cm/s, Pitch=%d deci_rad, Yaw=%d deci_rad", alt_int, spd_int, pit_int, yaw_int))
    else   
        gcs:send_text(3, "CAN Send Failed!")
    end

    return send_flight_data, 500 --repeats function every 500 ms
end

return send_flight_data() -- executes function

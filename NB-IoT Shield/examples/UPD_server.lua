-- upd-server Test
local socket = require "socket"

local port = 5683
local udp = socket.udp()
udp:settimeout(0)
udp:setsockname('*', port)    --server

local data, msg_or_ip, port_or_nil
local running = true

-- the beginning of the loop proper...
print "Beginning server loop."
while running do
    data, msg_or_ip, port_or_nil = udp:receivefrom()
    if data then
            print("udp:receivefrom: " .. data .. msg_or_ip, port_or_nil)
	    udp:sendto(data,msg_or_ip,port_or_nil)

            if data == "quit" then
                running = false
            end
    elseif msg_or_ip ~= 'timeout' then
        error("Unknown network error: "..tostring(msg))
    end

    socket.sleep(0.01)
end

print "Thank you."
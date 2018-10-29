package.path = package.path ..";?.lua;test/?.lua;app/?.lua;"

require "Pktgen"

local function doWait(port, waitTime)
    local idx;

   pktgen.delay(1000);

   if ( waitTime == 0 ) then
       return;
   end
   waitTime = waitTime - 1;

    -- Try to wait for the total number of packets to be sent.
    local idx = 0;
    while( idx < waitTime ) do

        idx = idx + 1;

        local sending = pktgen.isSending(port);
        if ( sending[tonumber(port)] == "n" ) then
            break;
        end
        pktgen.delay(1000);
    end
end

-- local function vary

pktgen.reset("all");

pktgen.delay(1000);
pktgen.src_ip("all", "start", "192.168.0.1");
pktgen.dst_ip("all", "start", "192.168.1.1");
pktgen.set_type("all", "ipv4");
pktgen.set_proto("all", "udp");

pktgen.start("all");
pktgen.doWait("all", 1);
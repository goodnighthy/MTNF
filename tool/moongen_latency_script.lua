local mg     = require "moongen"
local memory = require "memory"
local device = require "device"
local ts     = require "timestamping"
local hist   = require "histogram"
local stats  = require "stats"
local timer  = require "timer"
local log    = require "log"

-- set addresses here
local SRC_IP_BASE	= "192.168.0.1" -- actual address will be SRC_IP_BASE + random(0, flows)
local DST_IP		= "192.168.1.1"
local SRC_PORT		= 1234
local DST_PORT		= 5678

function configure(parser)
	parser:description("Generates UDP traffic and measure latencies. Edit the source to modify constants like IPs.")
	parser:argument("DevNo0", "Device Number 0."):convert(tonumber)
	parser:argument("DevNo1", "Device Number 1."):convert(tonumber)
	parser:option("-r --rate", "Transmit rate in Mbit/s."):default(2500):convert(tonumber)
	parser:option("-f --flows", "Number of flows (randomized source IP)."):default(5):convert(tonumber)
	parser:option("-o --offset", "Offset of flows (randomized source IP)."):default(5):convert(tonumber)
	parser:option("-s --size", "Packet size."):default(60):convert(tonumber)
end

function master(args)
	DevNo0 = device.config{port = args.DevNo0, rxQueues = 2, txQueues = 2}
	DevNo1 = device.config{port = args.DevNo1, rxQueues = 2, txQueues = 2}
	device.waitForLinks()
	-- max 1kpps timestamping traffic timestamping
	-- rate will be somewhat off for high-latency links at low rates
	if args.rate > 0 then
		DevNo0:getTxQueue(0):setRate(args.rate - (args.size + 4) * 8 / 1000)
		DevNo1:getTxQueue(0):setRate(args.rate - (args.size + 4) * 8 / 1000)
	end
	mg.startTask("loadSlave", DevNo0:getTxQueue(0), DevNo0, args.size, args.flows, args.offset * 0)
	mg.startTask("loadSlave", DevNo1:getTxQueue(0), DevNo1, args.size, args.flows, args.offset * 1)
	mg.startTask("timerSlave", DevNo0:getTxQueue(1), DevNo0:getRxQueue(1), args.size, args.flows, args.offset * 0)
	mg.startTask("timerSlave", DevNo1:getTxQueue(1), DevNo1:getRxQueue(1), args.size, args.flows, args.offset * 1)
	mg.waitForTasks()
end

local function fillUdpPacket(buf, len)
	buf:getUdpPacket():fill{
		ip4Src = SRC_IP,
		ip4Dst = DST_IP,
		udpSrc = SRC_PORT,
		udpDst = DST_PORT,
		pktLength = len
	}
end

function loadSlave(queue, rxDev, size, flows, offset)
	local mempool = memory.createMemPool(function(buf)
		fillUdpPacket(buf, size)
	end)
	local bufs = mempool:bufArray()
	local counter = 0
	local txCtr = stats:newDevTxCounter(queue, "plain")
	local rxCtr = stats:newDevRxCounter(rxDev, "plain")
	local baseIP = parseIPAddress(SRC_IP_BASE)
	while mg.running() do
		bufs:alloc(size)
		for i, buf in ipairs(bufs) do
			local pkt = buf:getUdpPacket()
			pkt.ip4.src:set(baseIP + offset + counter)
			counter = incAndWrap(counter, flows)
		end
		-- UDP checksums are optional, so using just IPv4 checksums would be sufficient here
		bufs:offloadUdpChecksums()
		queue:send(bufs)
		txCtr:update()
		rxCtr:update()
	end
	txCtr:finalize()
	rxCtr:finalize()
end

function timerSlave(txQueue, rxQueue, size, flows, offset)
	if size < 84 then
		log:warn("Packet size %d is smaller than minimum timestamp size 84. Timestamped packets will be larger than load packets.", size)
		size = 84
	end
	local timestamper = ts:newUdpTimestamper(txQueue, rxQueue)
	local hist = hist:new()
	mg.sleepMillis(1000) -- ensure that the load task is running
	local counter = 0
	local rateLimit = timer:new(0.001)
	local baseIP = parseIPAddress(SRC_IP_BASE)
	while mg.running() do
		hist:update(timestamper:measureLatency(size, function(buf)
			fillUdpPacket(buf, size)
			local pkt = buf:getUdpPacket()
			pkt.ip4.src:set(baseIP + offset + counter)
			counter = incAndWrap(counter, flows)
		end))
		rateLimit:wait()
		rateLimit:reset()
	end
	-- print the latency stats after all the other stuff
	mg.sleepMillis(300)
	hist:print()
	hist:save("histogram.csv")
end


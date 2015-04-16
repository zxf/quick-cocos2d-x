require("luapomelo")

--[[
--example
local json = require("framework.json")
local socket = require("socket")

local storage = {
}

pomelo.lib_init(pomelo.PC_LOG_DEBUG, nil, nil)
local client = pomelo.Client.new()

client:init(true, false, function(op, data)
    if op == pomelo.PC_LOCAL_STORAGE_OP_WRITE then
        storage = json.decode(data)
        return 0
    else
        return json.encode(storage)
    end
end)

client:connect('192.168.210.147', 3011)
recv = client:sub()
print(recv:result())
socket.sleep(2)
print(recv:result())
recv:close()

socket.sleep(1)
resp = client:request('connector.entryHandler.entry', '{"name": "test"}', 10)
socket.sleep(1)
print(resp:result())
resp:close()

resp = client:notify('test.testHandler.notify', '{"content": "test content"}', 10)
socket.sleep(1)
print(resp:result())
resp:close()

client:disconnect()

client:destroy()
pomelo.lib_cleanup()

]]
if not pomelo then 
    pomelo = {
        PC_LOCAL_STORAGE_OP_READ = 0,
        PC_LOCAL_STORAGE_OP_WRITE = 1,

        PC_RC_OK = 0,
        PC_RC_ERROR = -1,
        PC_RC_TIMEOUT = -2,
        PC_RC_INVALID_JSON = -3,
        PC_RC_INVALID_ARG = -4,
        PC_RC_NO_TRANS = -5,
        PC_RC_INVALID_THREAD = -6,
        PC_RC_TRANS_ERROR = -7,
        PC_RC_INVALID_ROUTE = -8,
        PC_RC_INVALID_STATE = -9,
        PC_RC_NOT_FOUND = -10,
        PC_RC_RESET = -11,

        PC_ST_NOT_INITED = 0,
        PC_ST_INITED = 1,
        PC_ST_CONNECTING = 2,
        PC_ST_CONNECTED = 3,
        PC_ST_DISCONNECTING = 4,
        PC_ST_UNKNOWN = 5,

        PC_LOG_DEBUG = 0,
        PC_LOG_INFO = 1,
        PC_LOG_WARN = 2,
        PC_LOG_ERROR = 3,
        PC_LOG_DISABLE = 4,

        PC_EV_USER_DEFINED_PUSH = 0,
        PC_EV_CONNECTED = 1,
        PC_EV_CONNECT_ERROR = 2,
        PC_EV_CONNECT_FAILED = 3,
        PC_EV_DISCONNECT = 4,
        PC_EV_KICKED_BY_SERVER = 5,
        PC_EV_UNEXPECTED_DISCONNECT = 6,
        PC_EV_PROTO_ERROR = 7,

        PC_EV_INVALID_HANDLER_ID = -1,

        PC_WITHOUT_TIMEOUT = -1,
    }

    local _lib_init = false

    function pomelo.lib_cleanup()
        local ret = 0
        if _lib_init then
            ret = luapomelo.lib_cleanup() 
            _lib_init = false
        end
        return ret
    end

    function pomelo.lib_init(log_level, ca_file, ca_path)
        if _lib_init then
            pomelo.lib_cleanup()
        end
        local ret = luapomelo.lib_init(log_level, ca_file, ca_path) 
        _lib_init = true
        return ret
    end


    pomelo.lib_cleanup = luapomelo.lib_cleanup
    pomelo.ev_to_str = luapomelo.ev_to_str
    pomelo.rc_to_str = luapomelo.rc_to_str
    pomelo.state_to_str = luapomelo.state_to_str

    local EventResponse = class("EventResponse")

    function EventResponse:ctor(client, event, buffer_size)
        self._client = client
        self._internal_data = luapomelo.sub(client._internal_data, event, buffer_size)
    end

    function EventResponse:close()
        luapomelo.sub_close(self._client._internal_data, self._internal_data)
        self._internal_data = nil
        self._client = nil
    end

    function EventResponse:result()
        return luapomelo.sub_result(self._internal_data)
    end

    local ResquestResponse = class("ResquestResponse")

    function ResquestResponse:ctor(client, route, msg, timeout)
        self._internal_data = luapomelo.request(client._internal_data, route, msg, timeout)
    end

    function ResquestResponse:close()
        luapomelo.request_close(self._internal_data)
        self._internal_data = nil
    end

    function ResquestResponse:result()
        return luapomelo.request_result(self._internal_data)
    end

    local NotifyResponse = class("NotifyResponse")

    function NotifyResponse:ctor(client, route, msg, timeout)
        self._internal_data = luapomelo.request(client._internal_data, route, msg, timeout)
    end

    function NotifyResponse:close()
        luapomelo.notify_close(self._internal_data)
        self._internal_data = nil
    end

    function NotifyResponse:result()
        return luapomelo.notify_result(self._internal_data)
    end

    pomelo.Client = class("Client")

    function pomelo.Client:ctor()
        if not _lib_init then
            pomelo.lib_init(pomelo.PC_LOG_WARN, nil, nil)
        end
        self._internal_data = nil
    end

    function pomelo.Client:init(use_tls, enable_poll, lc_callback)
        self._internal_data = luapomelo.create(use_tls, enable_poll, lc_callback)
        local ret = self._internal_data == nil
        if not ret then
            print("Pomelo client init.")
        end
        return ret
    end

    function pomelo.Client:connect(host, port)
        if self._internal_data then
            return luapomelo.connect(self._internal_data, host, port)
        else
            return nil
        end
    end

    function pomelo.Client:state()
        if self._internal_data then
            return luapomelo.state(self._internal_data)
        else
            return nil
        end
    end

    function pomelo.Client:sub(event, buffer_size)
        return EventResponse.new(self, event, buffer_size)
    end

    function pomelo.Client:request(route, msg, timeout)
        if self._internal_data then
            return ResquestResponse.new(self, route, msg, timeout)
        else
            return nil
        end
    end

    function pomelo.Client:notify(route, msg, timeout)
        if self._internal_data then
            return NotifyResponse.new(self, route, msg, timeout)
        else
            return nil
        end
    end

    function pomelo.Client:poll()
        if self._internal_data then
            return luapomelo.poll(self._internal_data)
        else
            return nil
        end
    end

    function pomelo.Client:quality()
        if self._internal_data then
            return luapomelo.quality(self._internal_data)
        else
            return nil
        end
    end

    function pomelo.Client:disconnect()
        if self._internal_data then
            return luapomelo.disconnect(self._internal_data)
        else
            return nil
        end
    end

    function pomelo.Client:destroy()
        if self._internal_data and luapomelo.destroy(self._internal_data) == pomelo.PC_RC_OK then
            self._internal_data = nil
            return true
        else
            return false
        end
    end
end

return pomelo


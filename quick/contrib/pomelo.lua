require("luapomelo")

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

  function pomelo.lib_init(log_level, ca_file, ca_path)
    local ret = luapomelo.lib_init(log_level, ca_file, ca_path) 
    _lib_init = true
    return ret
  end

  function pomelo.lib_cleanup()
    local ret = luapomelo.lib_cleanup() 
    _lib_init = false
    return ret
  end
  pomelo.lib_cleanup = luapomelo.lib_cleanup
  pomelo.ev_to_str = luapomelo.ev_to_str
  pomelo.rc_to_str = luapomelo.rc_to_str
  pomelo.state_to_str = luapomelo.state_to_str

  pomelo.Client = class("Client")

  function pomelo.Client:ctor()
    if not _lib_init then
      pomelo.lib_init(pomelo.PC_LOG_WARN, nil, nil)
    end
    self._internal_data = nil
  end

  function pomelo.Client:init(use_tls, enable_poll, lc_callback)
      self._internal_data = luapomelo.create(use_tls, enable_poll, lc_callback)
      return self._internal_data == nil
  end

  function pomelo.Client:connect(host, port)
      return luapomelo.connect(self._internal_data, host, port)
  end

  function pomelo.Client:state()
      return luapomelo.state(self._internal_data)
  end

  function pomelo.Client:add_ev_handler(handler)
      return luapomelo.add_ev_handler(self._internal_data, handler)
  end

  function pomelo.Client:rm_ev_handler(handler_id)
      return luapomelo.rm_ev_handler(self._internal_data, handler_id)
  end

  function pomelo.Client:request(route, msg, timeout, req_cb)
      return luapomelo.request(self._internal_data, route, msg, timeout, req_cb)
  end

  function pomelo.Client:notify(route, msg, timeout, notify_cb)
      return luapomelo.notify(self._internal_data, route, msg, timeout, notify_cb)
  end

  function pomelo.Client:poll()
      return luapomelo.poll(self._internal_data)
  end

  function pomelo.Client:quality()
      return luapomelo.quality(self._internal_data)
  end

  function pomelo.Client:disconnect()
      return luapomelo.disconnect(self._internal_data)
  end

  function pomelo.Client:destroy()
      if luapomelo.destroy(self._internal_data) == Client.PC_RC_OK then
        self._internal_data = nil
        return true
      else
        return false
      end
  end
end

return pomelo
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


  pomelo.lib_init = luapomelo.lib_init
  pomelo.lib_cleanup = luapomelo.lib_cleanup
  pomelo.ev_to_str = luapomelo.ev_to_str
  pomelo.rc_to_str = luapomelo.rc_to_str
  pomelo.state_to_str = luapomelo.state_to_str

  pomelo.Client = class("Client")

  function pomelo.Client:ctor()
    self._internal_data = nil
  end

  function pomelo.Client:init(use_tls, enable_poll, lc_callback)
      self._internal_data = luapomelo.create(use_tls, enable_poll, lc_callback);
      return self._internal_data == nil
  end
end

return pomelo
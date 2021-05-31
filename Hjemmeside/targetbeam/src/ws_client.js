var _ws;

var _url = null;

var _callbackHandlers = [];
var _connectedHandler = null;
var _disconnectedHandler = null;
var _usersUpdated = null;
var _pistolsUpdated = null;
var _showStatsHandler = null;

var _reconnecttimer;

var _transmitbuffer = [];

export function connect(url = null) {
  if (url != null) {
    _url = url;
  }
  _ws = new WebSocket(_url);

  _ws.addEventListener("close", event => {
    console.log("Connection closed: ", event);
    if (_disconnectedHandler != null) {
      _disconnectedHandler();
    } else {
      console.log("No disconnect handler assigned.");
    }
    startReconnectTimer();
  });

  _ws.addEventListener("error", event => {
    console.log("Connection error: ", event);
    startReconnectTimer();
  });

  _ws.addEventListener("message", event => {
    var message;
    try {
      message = JSON.parse(event.data);
      console.log("Message recieved: ", message);
      handleMessage(message);
    } catch {
      message = event.data;
      console.log("Message recieved: ", message);
    }
  });

  _ws.addEventListener("open", event => {
    console.log("Connection established: ", event);
    send({
      type: "RequestConnectionVerification",
      connectionType: "browser"
    });

    for (var i = 0; i < _transmitbuffer.length; i++) {
      var message = _transmitbuffer.shift();
      send(message.message, message.callback);
    }
  });
}

function startReconnectTimer() {
  clearTimeout(_reconnecttimer);
  _reconnecttimer = setTimeout(() => {
    _ws.close();
    connect();
  }, 3000);
}

export function addDisconnectedHandler(disconnectedHandler) {
  _disconnectedHandler = disconnectedHandler;
}

export function addConnectedHandler(connectedHandler) {
  _connectedHandler = connectedHandler;
}

export function addUsersUpdated(usersUpdated) {
  _usersUpdated = usersUpdated;
}

export function addPistolsUpdated(pistolsUpdated) {
  _pistolsUpdated = pistolsUpdated;
}

export function addShowStatsHandler(showStatsHandler) {
  _showStatsHandler = showStatsHandler;
}

export function close() {
  _ws.close();
}

function handleMessage(message) {
  switch (message.type) {
    case "SignedInUsers": // List of connected users
      if (_usersUpdated != null) {
        _usersUpdated(message.data);
      } else {
        console.log(
          "No users updated handler assigned. Data will not be processed.",
          message.data
        );
      }
      break;
    case "Pistollist": // List of connected users
      if (_pistolsUpdated != null) {
        _pistolsUpdated(message.pistols);
      } else {
        console.log(
          "No pistols updated handler assigned. Data will not be processed.",
          message.data
        );
      }
      break;
    case "SetToDormantBrowser": // List of connected users
      if (_connectedHandler != null) {
        _connectedHandler(false);
      } else {
        console.log(
          "No connected handler assigned. Data will not be processed.",
          message.data
        );
      }
      break;
    case "UpgradeToPrimaryBrowser": // List of connected users
      if (_connectedHandler != null) {
        _connectedHandler(true);
      } else {
        console.log(
          "No connected handler assigned. Data will not be processed.",
          message.data
        );
      }
      break;
    case "ShowStats": // List of connected users
      if (_showStatsHandler != null) {
        _showStatsHandler(true);
      } else {
        console.log(
          "No ShowStats handler assigned. Data will not be processed.",
          message.data
        );
      }
      break;
    case "Callback": // Callback to some message
      var CBH = _callbackHandlers.filter(CBH => {
        return CBH.id == message.callback;
      });
      if (CBH.length > 0) {
        CBH[0].cb(message);
        _callbackHandlers = _callbackHandlers.filter(CBH => {
          return CBH.id != message.callback;
        });
      } else {
        console.log(
          "No callback handler assigned to callbackID '" +
          message.callback +
          "'. Data will not be processed.",
          message.data
        );
      }
      break;
    default:
      break;
  }
}

export function send(message, callback = null) {
  if (_ws.readyState) {
    if (callback != null) {
      var cbid = Math.floor(Math.random() * 1000) + 1;
      while (
        _callbackHandlers.filter(CBH => {
          return CBH.id == cbid;
        }).length > 0
      ) {
        cbid = Math.floor(Math.random() * 1000) + 1;
      }
      message.callback = cbid;
      _callbackHandlers.push({ id: cbid, cb: callback });
    }
    _ws.send(JSON.stringify(message));
    console.log("Transmitted ", message);
  } else {
    console.log("Connection not established to '" + _url + "'");
    _transmitbuffer.push({
      message: message,
      callback: callback,
      timestamp: Date.now()
    });
  }
}

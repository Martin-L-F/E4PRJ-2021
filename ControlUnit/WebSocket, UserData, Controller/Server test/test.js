const { exit } = require('process');
const { setTimeout } = require('timers');
const WebSocket = require('ws');

var server_uri = "ws://localhost:3000";

function test1() {
    console.log(" ================================== ");
    console.log(" === Test 1: Connect to server  === ");
    console.log(" ================================== ");

    var testComplete = false;
    var testState = {
        state: 0,
        goal: 1
    }

    var ws = new WebSocket(server_uri);

    ws.addEventListener('open', event => {
        console.log("👍 Connected");
        testComplete = true;
        testState.state++;
    });

    ws.addEventListener('error', event => {
        console.log("❌ Error", event);
        testComplete = true;
    });
}

test1();

function wait() {
    if (!testComplete) {
        setTimeout(wait, 50);
        return;
    }
    ws.close();
    if (testState.goal == testState.state) {
        console.log("Test succeeded (" + testState.state + "/" + testState.goal + ")")
    } else {
        console.log("Test failed (" + testState.state + "/" + testState.goal + ")")
    }
    exit();
}
wait();
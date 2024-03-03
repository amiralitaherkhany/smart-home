
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
    initButton();
}

function getReadings() {
    websocket.send("getReadings");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++) {
        var key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
    }
}
function initButton() {
    document.getElementById('button1').addEventListener('click', toggle1);
    document.getElementById('button2').addEventListener('click', toggle2);
}
function toggle1() {
    console.log('toggle1');
    websocket.send('toggle');
}
function toggle2() {
    console.log('toggle2');
    websocket.send('fan');
}

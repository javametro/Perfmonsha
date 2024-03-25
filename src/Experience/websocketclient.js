require('websocket')

const ws = new WebSocket('ws://localhost:8080');

//Handle incoming messages
ws.addEventListener('message', (event) => {
	const message = event.data;
	
	//handle the message (SDP, ICE candidate)
	console.log(message);
});

//Send messages to the server
ws.send('Hello from the client');
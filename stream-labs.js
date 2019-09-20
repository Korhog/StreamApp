const config = require('./config');
const EventEmmiter = require('events');
const io = require('socket.io-client');

class StreamLabs extends EventEmmiter {
    constructor() {
        super();

        var socketToken = config.StreamLabs;
        var url = `https://sockets.streamlabs.com?token=${socketToken}`
        this.socket = io.connect(url, {transports: ['websocket']});

        this.socket.on('event', this.onEvent);
        this.socket.on('event', (data) => {
            console.log(data);
        });

    }

    onEvent(data) {
        console.log(data);
    }
}

module.exports = StreamLabs;
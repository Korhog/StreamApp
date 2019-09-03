const EventEmmiter = require('events');
const MixerNode = require('./mixer-interactive');
const MixerChat = require('./mixer-chat');

class Mixer extends EventEmmiter {
    constructor() {
        super();
        this.interactive = new MixerNode();
        this.interactive.on('event', (data) => {
            this.emit('event', data);
        });

        this.chat = new MixerChat();
        this.chat.on('event', (data) => {
            this.emit('chat', data);
        });      
    }
};

module.exports = Mixer;
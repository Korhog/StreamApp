const EventEmmiter = require('events');
const MixerNode = require('./mixer-interactive');

class Mixer extends EventEmmiter {
    constructor() {
        super();
        this.interactive = new MixerNode();
        console.log('Mixer is here');
    }
};

module.exports = Mixer;
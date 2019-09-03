const config = require('./config');
const Mixer = require('./mixer');

const mixerNode = new Mixer();
mixerNode.on('event', (data) => {
    if (data.eventType === 'onClick') {
        mixerNode.chat.sendMessage(`Mixer interactive: button ${data.eventData.buttonID} is pressed`)
    }
});

mixerNode.on('chat', (data) => {
    console.log(`Mixer chat: someone sayed ${data.message.message[0].data}`);    
});
const config = require('./config');
const Mixer = require('./mixer');
const Serial = require('serial-node');
const serial = new Serial(9600);
serial.use('COM3');
serial.open();



const mixerNode = new Mixer();
mixerNode.on('event', (data) => {
    if (data.eventType === 'onClick') {
        if (data.eventData.userInfo.level > 40) {
            mixerNode.chat.sendMessage(`Mixer interactive: button ${data.eventData.buttonID} is pressed`)
            if (data.eventData.buttonID == 'colors::red') {
                serial.write(1);
            }

            if (data.eventData.buttonID == 'color::green') {
                serial.write(2);
            }

            
            if (data.eventData.buttonID == 'rainbow') {
                serial.write(3);
            }  
        }      
    }
});

mixerNode.on('chat', (data) => {
    console.log(`Mixer chat: someone sayed ${data.message.message[0].data}`);   
    if (data.userInfo.ascensionLevel > 5) {
        if (data.message.message[0].data == '!rainbow') {
            serial.write(3);
        } 
    }
});
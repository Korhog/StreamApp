const config = require('./config');
const EventEmmiter = require('events');

const Mixer = require('@mixer/client-node');
const ws = require('ws');

class MixerChat extends EventEmmiter {
    constructor() {
        super();

        this.client = new Mixer.Client(new Mixer.DefaultRequestRunner());
        this.client.use(new Mixer.OAuthProvider(this.client, {
            tokens: {
                access: config.MixerChatId,
                expires: Date.now() + (365 * 24 * 60 * 60 * 1000)
            }
        }));

        this.client.request('GET', 'users/current').then(response => {
            console.log(response.body);
            this.userInfo = response.body;       
            
            return new Mixer.ChatService(this.client).join(response.body.channel.id);
        }).then(response => {
            const body = response.body;
            this.createChatSocket(
                this.userInfo.id, 
                this.userInfo.channel.id,
                body.endpoints,
                body.authkey
            );
        }).catch(err => console.log(err)); 
    }

    async createChatSocket(userId, channelId, endpoints, authkey) {
        this.socket = new Mixer.Socket(ws, endpoints).boot();
        this.socket.auth(channelId, userId, authkey)
            .then(() => {
                console.log('socket created');
            })
            .catch(err => console.log(err));

                    // Listen for chat messages. Note you will also receive your own!
        this.socket.on('ChatMessage', data => {
            console.log(data);
            this.emit('event', {
                userInfo: {
                    ascensionLevel: data.user_ascension_level
                },
                message: data.message
            });
        });

        // Listen for socket errors. You will need to handle these here.
        this.socket.on('error', error => console.error(error));  
    }

    async sendMessage(message) {
        if (this.socket) {
            this.socket.call('msg', [message]);
        }
    }
}

module.exports = MixerChat;
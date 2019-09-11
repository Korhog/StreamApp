const config = require('./config');
const fs = require('fs');
const ws = require('ws');

const EventEmmiter = require('events');
const { ShortCodeExpireError, OAuthClient } = require('@mixer/shortcode-oauth');
const interactive = require('@mixer/interactive-node');

class MixerNode extends EventEmmiter {
    constructor() {
        super();
        this.participants = {};
        // create OAuth client
        this.oauth = new OAuthClient({
            clientId: config.MixerGameId,
            scopes: ['interactive:robot:self'],
            });  

        fs.readFile('local.token', 'utf8', (err, data) => {
            if (err) {
                this.shortCode();
            }
            else {
                this.refreshToken(data);
            }
        });
    }

    // get token bu short code
    async shortCode() {
          
        this.oauth.getCode()
            .then(code => {
                console.log(`Go to mixer.com/go and enter ${code.code}`);
                return code.waitForAccept();
            })
            .then(token => {
                this.token = token;
                this.saveToken(token).then(result => {
                    console.log("Token saved");
                })                           
            })
            .catch(err => console.log(err));
    }

    // if token exists, try to refresh
    async refreshToken(data) {
        var token = JSON.parse(data);
        this.oauth.refresh(token).then(token => {
            this.token = token;
            this.saveToken(token).then(result => {
                console.log("Token saved");
            });

            this.createGame(token);
        })
        .catch(err => console.log(err));
    }

    getUserInfo(participantID) {
        var userInfo = this.game.state.getParticipantBy(participantID);
        if (userInfo) {
            return {
                level: userInfo.level
            }
        }
    }

    async saveToken(token) {
        var sToken = JSON.stringify(token);
        return fs.writeFile('local.token', sToken, (err) => {
            if (err) {
                return false
            }
            return true;
        });
    }

    async createGame(token) {
        // set webSocket
        interactive.setWebSocket(ws);
        this.game = new interactive.GameClient();

        // subscribing to events
        this.game.on('open', () => {
            console.log('Connected to interactive');
            this.game.ready(true);
            

            
            this.game.getScenes().then(scenes => {
                var controls = scenes.scenes[0].controls;
                controls.forEach(control => {
                    if (control.kind === 'button') {
                        
                        // control.on('mousedown', (event, participant) => {
                        //     console.log(event);
                        //     console.log(participant);
                        // });
                    }
                });                
            });
        });

        this.game.state.on('participantJoin', p => {
            console.log(this.game.state.participants)
        });

        this.game.on('message', (sData) => {
            var data = JSON.parse(sData);            
            if (data.method === "giveInput") { 
                var input = data.params.input;               
                if (input.event === 'mousedown') {                  
                    this.emit('event', {
                        eventType: 'onClick',
                        eventData: {
                            userInfo: this.getUserInfo(data.params.participantID),
                            buttonID: input.controlID
                        } 
                    });
                }
            }
        });
        

        // open game client with oauth
        this.game.open({
            authToken: token.data.accessToken,
            versionId: config.ClientId,
        });
    }
}

module.exports = MixerNode;
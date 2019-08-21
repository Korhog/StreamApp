const config = require('./config');
const fs = require('fs');
const ws = require('ws');

const EventEmmiter = require('events');
const { ShortCodeExpireError, OAuthClient } = require('@mixer/shortcode-oauth');
const interactive = require('@mixer/interactive-node');

class MixerNode extends EventEmmiter {
    constructor() {
        super();

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
        this.game.on('open', () => console.log('Connected to interactive'));
        this.game.on('message', (sData) => {

            var data = JSON.parse(sData);            
            if (data.method === "giveInput") {
                console.log('<<<', data.params.input);
            }
        });
        

        // open game client with oauth
        this.game.open({
            authToken: token.data.accessToken,
            versionId: config.ClientId,
        }).then(() => {
            this.game.ready(true);
        })
    }
}

module.exports = MixerNode;
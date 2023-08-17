const osc = require('osc');

var udpPort = new osc.UDPPort({
    remoteAddress: '127.0.0.1',
    remotePort: 8000,
    metadata: true,
});

// Open the socket.
udpPort.open();

function midi(value: number) {
    return {
        type: 'm',
        value: new Uint8Array([value]),
    };
}

udpPort.send({ address: '/midi', args: [midi(0xb0), midi(0x48), midi(0x10)] });
udpPort.send({ address: '/midi', args: [midi(0xc0), midi(0x48)] });

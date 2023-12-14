const express = require('express');
const http = require('http');
const WebSocket = require('ws');

// const fs = require('fs');
const path = require('path');
const fsp = require('fs').promises;
const fs = require('fs')
// const fetch = require('node-fetch');

const { exec } = require('child_process');
const { promisify } = require('util');

const execAsync = promisify(exec);

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

app.use(express.static('public'));
const filePath = 'fun_info.txt';

wss.on('connection', (ws) => {
    ws.on('message', async (message) => {
        const data = JSON.parse(message);
        if (data.action === 'readFile') {
            // Read the text file and send its content to the client
            try {
                  const fileContent = await readFile(filePath);
                  ws.send(JSON.stringify({ code: true, fileContent }));
              } catch (error) {
                  console.error(`Error reading file: ${error.message}`);
                  ws.send(JSON.stringify({ message: 'Error reading file' }));
              }


        } else {
            const { code } = JSON.parse(message);

            // Save the user's code to a file in the 'src' folder
            const fileName = path.join(__dirname, 'sample.c');
            fs.writeFileSync(fileName, code);
            const execute_command = './Executer.exe'
            const { stdout, stderr } = await execAsync(execute_command);


            // const compileCommand = `clang -g ${fileName} -o tempCode`;
            //
            // exec(compileCommand, (compileError, compileStdout, compileStderr) => {
            //     if (compileError) {
            //         console.error(`Error compiling code: ${compileError.message}`);
            //         ws.send(JSON.stringify({ message: 'Error compiling code' }));
            //         return;
            //     }
            //
            //     // Run readelf to get stack size information
            //     const readelfCommand = `readelf --headers tempCode`;
            //
            //     exec(readelfCommand, (readelfError, readelfStdout) => {
            //         if (readelfError) {
            //             console.error(`Error running readelf: ${readelfError.message}`);
            //             ws.send(JSON.stringify({ message: 'Error obtaining stack sizes' }));
            //             return;
            //         }
            //
            //         const result = compileStdout || compileStderr;
            //         const stackSizes = extractStackSizes(readelfStdout);
            //         ws.send(JSON.stringify({ message: result, stackSizes }));


            ws.send(JSON.stringify({ code: false, result: stdout, error: stderr }));
            //
            //     });
            // });
        }
    });
});

async function readFile(filePath) {
    try {
        const fileContent = await fsp.readFile(filePath, 'utf-8');
        return fileContent;
    } catch (error) {
        throw new Error(`Error reading file: ${error.message}`);
    }
}



function extractStackSizes(output) {
    const regex = /Function:\s+(\w+)\s+StackSize:\s+(\d+)\s+MaxAllocSize:\s+(\d+)/g;
    const stackSizes = {};
    let match;

    while ((match = regex.exec(output)) !== null) {
        const functionName = match[1];
        const stackSize = match[2];
        stackSizes[functionName] = parseInt(stackSize);
    }

    return stackSizes;
}

server.listen(3000, () => {
    console.log('Server is running on port 3000');
});

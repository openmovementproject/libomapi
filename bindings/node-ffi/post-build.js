/*
const path = require('path');
const {spawnSync} = require('child_process');
let command = (process.platform === 'win32') ? 'post-build.cmd' : './post-build'
console.log('spawn: ', command);
let child = spawnSync(command);
console.log('error: ', child.error);
console.log('stdout: ', child.stderr.toString());
console.log('stderr: ', child.stderr.toString());
*/

const fs = require('fs');
const path = require('path');

const lib = path.join('.', 'lib'); // __dirname
if (!fs.existsSync(lib)) {
    console.log('POST-MKDIR: ' + lib);
    fs.mkdirSync(lib);
}

const binding = path.join(lib, 'binding');
if (!fs.existsSync(binding)) {
    console.log('POST-MKDIR: ' + binding);
    fs.mkdirSync(binding);
}

let srcFilename;
let destFilename;

if (process.platform === 'win32') {
    srcFilename = 'libomapi.node';
    destFilename = 'libomapi.dll';
} else if (process.platform === 'darwin') {
    srcFilename = 'omapi.node';
    destFilename = 'libomapi.dylib';
} else {
    srcFilename = 'omapi.node';
    destFilename = 'libomapi.so';
}
    
const src = path.join('build', 'Release', srcFilename);
const dest = path.join(binding, destFilename);

console.log('POST: (' + process.platform + ') ' + srcFilename + ' -> ' + destFilename);

if (!fs.existsSync(dest)) {
    console.log('POST-REMOVE-EXISTING: ' + dest);
    fs.unlinkSync(dest);
}

if (!fs.existsSync(src))
{
    throw new Error('ERROR: Source file does not exist: ' + src);
}

console.log('POST-RENAME: ' + src + ' -> ' + dest);
fs.renameSync(src, dest); 

const lucide = require('lucide');
const allKeys = Object.keys(lucide.icons);

const originalCount = 300;
const originalCommon = ['Home', 'User', 'Settings', 'Search', 'Menu', 'Heart', 'Star', 'Check', 'Bell', 'Calendar', 'Camera', 'Mail', 'Phone', 'Image', 'Trash', 'Edit', 'Save', 'Upload', 'Download', 'Link', 'Lock', 'Eye', 'Info'];
const originalIcons = [];

for (const c of originalCommon) {
  if (allKeys.includes(c) && !originalIcons.includes(c)) {
    originalIcons.push(c);
  }
}

const step300 = Math.floor(allKeys.length / originalCount);
for (let i = 0; i < allKeys.length && originalIcons.length < originalCount; i += step300) {
  if (!originalIcons.includes(allKeys[i])) {
    originalIcons.push(allKeys[i]);
  }
}

let idx = 0;
while (originalIcons.length < originalCount && idx < allKeys.length) {
  if (!originalIcons.includes(allKeys[idx])) {
    originalIcons.push(allKeys[idx]);
  }
  idx++;
}

console.log("Original 300 icons count:", originalIcons.length);

const targetCount = 800;
const newCommon = ['Home', 'User', 'Settings', 'Search', 'Menu', 'Heart', 'Star', 'Check', 'Bell', 'Calendar', 'Camera', 'Mail', 'Phone', 'Image', 'Trash', 'Edit', 'Save', 'Upload', 'Download', 'Link', 'Lock', 'Eye', 'Info', 'Cpu', 'Computer', 'Gpu', 'Microchip', 'HardDrive', 'Server', 'Database', 'Cloud', 'Network', 'Router', 'MemoryStick', 'Terminal', 'Monitor', 'GitBranch', 'Branch', 'TreePine', 'TreeDeciduous', 'Leaf', 'Plant', 'Gamepad', 'Headphones', 'Speaker', 'Mic', 'Video', 'Film', 'Music'];

const finalIcons = [...originalIcons];

for (const c of newCommon) {
  if (allKeys.includes(c) && !finalIcons.includes(c)) {
    finalIcons.push(c);
  }
}

const step800 = Math.floor(allKeys.length / targetCount);
for (let i = 0; i < allKeys.length && finalIcons.length < targetCount; i += step800) {
  if (!finalIcons.includes(allKeys[i])) {
    finalIcons.push(allKeys[i]);
  }
}

idx = 0;
while (finalIcons.length < targetCount && idx < allKeys.length) {
  if (!finalIcons.includes(allKeys[idx])) {
    finalIcons.push(allKeys[idx]);
  }
  idx++;
}

console.log("Final icons count:", finalIcons.length);

const fs = require('fs');
fs.writeFileSync('generate_icons.js', fs.readFileSync('generate_icons.js', 'utf-8').replace(
  /const availableIcons = \[\];[\s\S]*?(?=for \(const icon of availableIcons\))/m,
  `const availableIcons = ${JSON.stringify(finalIcons)};\n\n`
));

console.log("Patched generate_icons.js with hardcoded list to preserve all original dependencies.");

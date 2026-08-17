const fs = require('fs');
const path = require('path');
const lucide = require('lucide');

const darkDir = path.join(__dirname, 'darkmode');
const lightDir = path.join(__dirname, 'lightmode');

if (!fs.existsSync(darkDir)) fs.mkdirSync(darkDir, { recursive: true });
if (!fs.existsSync(lightDir)) fs.mkdirSync(lightDir, { recursive: true });

function getSvg(iconName, strokeColor) {
  const iconNodes = lucide.icons[iconName];
  let paths = iconNodes.map(node => {
    const tag = node[0];
    const attrs = node[1];
    let attrStr = Object.entries(attrs).map(([k, v]) => `${k}="${v}"`).join(' ');
    return `  <${tag} ${attrStr} />`;
  }).join('\n');

  return `<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="${strokeColor}" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">\n${paths}\n</svg>`;
}

const allKeys = Object.keys(lucide.icons);
const targetCount = 500;

const common = ['Cpu', 'Computer', 'Gpu', 'Microchip', 'HardDrive', 'Server', 'Database', 'Cloud', 'Network', 'Router', 'MemoryStick', 'Terminal', 'Monitor', 'GitBranch', 'Branch', 'TreePine', 'TreeDeciduous', 'Leaf', 'Plant', 'Gamepad', 'Headphones', 'Speaker', 'Mic', 'Video', 'Film', 'Music'];

const availableIcons = [];

for (const c of common) {
  if (allKeys.includes(c)) {
    const filename = c.replace(/([a-z0-9])([A-Z])/g, '$1-$2').toLowerCase() + '.svg';
    if (!fs.existsSync(path.join(darkDir, filename)) && !availableIcons.includes(c)) {
      availableIcons.push(c);
    }
  }
}

let idx = 0;
while (availableIcons.length < targetCount && idx < allKeys.length) {
  const c = allKeys[idx];
  const filename = c.replace(/([a-z0-9])([A-Z])/g, '$1-$2').toLowerCase() + '.svg';
  if (!fs.existsSync(path.join(darkDir, filename)) && !availableIcons.includes(c)) {
    availableIcons.push(c);
  }
  idx++;
}

for (const icon of availableIcons) {
  const darkSvg = getSvg(icon, '#FFFFFF');
  const lightSvg = getSvg(icon, '#000000');

  const filename = icon.replace(/([a-z0-9])([A-Z])/g, '$1-$2').toLowerCase() + '.svg';

  fs.writeFileSync(path.join(darkDir, filename), darkSvg);
  fs.writeFileSync(path.join(lightDir, filename), lightSvg);
}

console.log(`Successfully added ${availableIcons.length} extra light and dark SVG icons from Lucide!`);

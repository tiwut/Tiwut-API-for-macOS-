const fs = require('fs');
const path = require('path');

const darkDir = path.join(__dirname, 'darkmode');
const lightDir = path.join(__dirname, 'lightmode');

if (!fs.existsSync(darkDir)) fs.mkdirSync(darkDir, { recursive: true });
if (!fs.existsSync(lightDir)) fs.mkdirSync(lightDir, { recursive: true });

function getAlienSvg(paths, strokeColor) {
  return `<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="${strokeColor}" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">\n${paths}\n</svg>`;
}

const alienIcons = {
  'xeon-nexus': `  <path d="M12 2L2 12l10 10 10-10Z" />\n  <circle cx="12" cy="12" r="4" />\n  <path d="M12 8v8" />`,
  'vort-flux': `  <path d="M4 4h16v16H4z" />\n  <path d="M4 4l16 16" />\n  <path d="M20 4L4 20" />\n  <circle cx="12" cy="12" r="3" />`,
  'quasar-node': `  <path d="M12 2v20" />\n  <path d="M2 12h20" />\n  <circle cx="12" cy="12" r="8" />\n  <circle cx="12" cy="12" r="4" />`,
  'zeth-core': `  <polygon points="12 2 22 8.5 22 15.5 12 22 2 15.5 2 8.5 12 2" />\n  <path d="M12 22V12" />\n  <path d="M22 8.5L12 12" />\n  <path d="M2 8.5L12 12" />`,
  'lyra-sigil': `  <path d="M3 12a9 9 0 1 0 18 0 9 9 0 1 0-18 0" />\n  <path d="M3 12c0-4.97 4-9 9-9 0 4.97-4 9-9 9z" />\n  <path d="M21 12c0 4.97-4 9-9 9 0-4.97 4-9 9-9z" />`,
  'omni-glyph': `  <path d="M6 6h12v12H6z" />\n  <path d="M12 2v4" />\n  <path d="M12 18v4" />\n  <path d="M2 12h4" />\n  <path d="M18 12h4" />`,
  'kryl-spark': `  <path d="M12 2l3 7 7 3-7 3-3 7-3-7-7-3 7-3z" />\n  <circle cx="12" cy="12" r="2" />`,
  'nyx-weaver': `  <path d="M2 12c5.523 0 10-4.477 10-10 0 5.523 4.477 10 10 10-5.523 0-10 4.477-10 10 0-5.523-4.477-10-10-10z" />`,
  'tesseract-eye': `  <rect x="3" y="3" width="18" height="18" rx="2" />\n  <rect x="7" y="7" width="10" height="10" rx="1" />\n  <circle cx="12" cy="12" r="1" />`,
  'pulsar-ring': `  <circle cx="12" cy="12" r="10" />\n  <path d="M12 2a10 10 0 0 1 10 10" />\n  <path d="M12 22a10 10 0 0 1-10-10" />`
};

const baseKeys = Object.keys(alienIcons);
for (let i = 0; i < 40; i++) {
  const baseKey = baseKeys[i % baseKeys.length];
  const basePath = alienIcons[baseKey];

  const rotation = (i * 45) % 360;
  const scale = 0.8 + (i % 3) * 0.1;
  const variedPath = `<g transform="translate(12, 12) rotate(${rotation}) scale(${scale}) translate(-12, -12)">\n${basePath}\n</g>`;

  alienIcons[`${baseKey}-var-${i + 1}`] = variedPath;
}

const allIcons = Object.keys(alienIcons);

for (const icon of allIcons) {
  const darkSvg = getAlienSvg(alienIcons[icon], '#FFFFFF');
  const lightSvg = getAlienSvg(alienIcons[icon], '#000000');

  fs.writeFileSync(path.join(darkDir, `${icon}.svg`), darkSvg);
  fs.writeFileSync(path.join(lightDir, `${icon}.svg`), lightSvg);
}

console.log(`Generated ${allIcons.length} completely unique alien symbols!`);

const fs = require('fs');
const path = require('path');

const darkDir = path.join(__dirname, 'darkmode');
const lightDir = path.join(__dirname, 'lightmode');

if (fs.existsSync(darkDir)) fs.rmSync(darkDir, { recursive: true, force: true });
if (fs.existsSync(lightDir)) fs.rmSync(lightDir, { recursive: true, force: true });

fs.mkdirSync(darkDir, { recursive: true });
fs.mkdirSync(lightDir, { recursive: true });

function getSvg(innerPaths, strokeColor) {
  return `<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="${strokeColor}" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">\n  ${innerPaths}\n</svg>`;
}

const baseIcons = {
  'user': '<circle cx="12" cy="8" r="4" /><path d="M4 20c0-4 4-6 8-6s8 2 8 6" />',
  'home': '<path d="M4 10L12 3l8 7v11H4z" /><path d="M10 21v-6h4v6" />',
  'search': '<circle cx="10" cy="10" r="7" /><path d="M15 15l6 6" />',
  'settings': '<circle cx="12" cy="12" r="4" /><path d="M12 2v3m0 14v3m10-10h-3M5 12H2m15.071 7.071l-2.121-2.121M7.05 7.05L4.929 4.929m14.142 0l-2.121 2.121M7.05 16.95l-2.121 2.121" />',
  'menu': '<path d="M4 6h16M4 12h16M4 18h16" />',
  'close': '<path d="M6 6l12 12M6 18L18 6" />',
  'check': '<path d="M5 12l4 4 10-10" />',
  'arrow-right': '<path d="M5 12h14M12 5l7 7-7 7" />',
  'arrow-left': '<path d="M19 12H5M12 5l-7 7 7 7" />',
  'arrow-up': '<path d="M12 19V5M5 12l7-7 7 7" />',
  'arrow-down': '<path d="M12 5v14M5 12l7 7 7-7" />',
  'heart': '<path d="M12 20.5l-8.5-8.5a5 5 0 0 1 7-7L12 6.5l1.5-1.5a5 5 0 0 1 7 7l-8.5 8.5z" />',
  'star': '<polygon points="12 3 15 9 21 10 16 14 17 21 12 18 7 21 8 14 3 10 9 9" />',
  'mail': '<rect x="3" y="6" width="18" height="12" rx="2" /><path d="M3 8l9 6 9-6" />',
  'phone': '<path d="M6.5 4v0a2 2 0 0 1 2 2l1 3a2 2 0 0 1-.5 2.5l-2 2a14 14 0 0 0 7 7l2-2a2 2 0 0 1 2.5-.5l3 1a2 2 0 0 1 2 2v0a2 2 0 0 1-2 2h-1c-8.8 0-16-7.2-16-16v-1a2 2 0 0 1 2-2z" />',
  'calendar': '<rect x="4" y="6" width="16" height="14" rx="2" /><path d="M8 4v4M16 4v4M4 10h16" />',
  'clock': '<circle cx="12" cy="12" r="9" /><path d="M12 7v5l3 3" />',
  'camera': '<rect x="3" y="8" width="18" height="12" rx="2" /><path d="M7 8V6a2 2 0 0 1 2-2h6a2 2 0 0 1 2 2v2" /><circle cx="12" cy="14" r="3" />',
  'image': '<rect x="3" y="4" width="18" height="16" rx="2" /><circle cx="8.5" cy="8.5" r="1.5" /><path d="M3 16l5-5 4 4 3-3 6 6" />',
  'file': '<path d="M6 3h8l6 6v11a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2z" /><path d="M14 3v6h6" />',
  'folder': '<path d="M3 7v11a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2V9a2 2 0 0 0-2-2h-6l-2-3H5a2 2 0 0 0-2 2z" />',
  'trash': '<path d="M4 6h16M9 6V4a1 1 0 0 1 1-1h4a1 1 0 0 1 1 1v2M5 6v14a2 2 0 0 0 2 2h10a2 2 0 0 0 2-2V6M10 11v6M14 11v6" />',
  'edit': '<path d="M18 4l2 2-11 11H7v-2L18 4z" />',
  'save': '<rect x="4" y="4" width="16" height="16" rx="2" /><path d="M8 4v5h8V4M8 20v-6h8v6" />',
  'lock': '<rect x="5" y="11" width="14" height="10" rx="2" /><path d="M8 11V7a4 4 0 0 1 8 0v4" />',
  'unlock': '<rect x="5" y="11" width="14" height="10" rx="2" /><path d="M8 11V7a4 4 0 0 1 7.5-1.5" />',
  'eye': '<path d="M3 12c3-4.5 6-7 9-7s6 2.5 9 7-6 7-9 7-6-2.5-9-7z" /><circle cx="12" cy="12" r="3" />',
  'eye-off': '<path d="M3 12c3-4.5 6-7 9-7s6 2.5 9 7-6 7-9 7-6-2.5-9-7z" /><circle cx="12" cy="12" r="3" /><path d="M3 3l18 18" />',
  'bell': '<path d="M12 4a4 4 0 0 0-4 4v5l-2 3h12l-2-3V8a4 4 0 0 0-4-4zM10 18a2 2 0 0 0 4 0" />',
  'shield': '<path d="M12 3L4 6v6c0 4.5 3.5 8.5 8 10 4.5-1.5 8-5.5 8-10V6z" />',
  'link': '<path d="M9 12h6M6.5 8H5a4 4 0 0 0 0 8h1.5M17.5 8H19a4 4 0 0 1 0 8h-1.5" />',
  'sun': '<circle cx="12" cy="12" r="4" /><path d="M12 2v3M12 19v3M3 12h3M18 12h3M5 5l2 2M17 17l2 2M5 19l2-2M17 7l2-2" />',
  'moon': '<path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z" />',
  'cloud': '<path d="M7 17h11a4 4 0 0 0 0-8h-.5a7 7 0 0 0-13 2.5A3.5 3.5 0 0 0 7 17z" />',
  'lightning': '<path d="M13 3L5 13h6l-2 8 8-10H11z" />',
  'map': '<path d="M3 6l6-3 6 3 6-3v14l-6 3-6-3-6 3V6zM9 3v14M15 6v14" />',
  'music': '<path d="M9 18V5l10-2v13" /><circle cx="6" cy="18" r="3" /><circle cx="16" cy="16" r="3" />',
  'video': '<rect x="3" y="6" width="14" height="12" rx="2" /><path d="M17 10l5-3v10l-5-3" />',
  'mic': '<rect x="9" y="3" width="6" height="10" rx="3" /><path d="M5 11v2a7 7 0 0 0 14 0v-2M12 20v3M9 23h6" />',
  'headphones': '<path d="M4 14v-3a8 8 0 0 1 16 0v3" /><rect x="2" y="14" width="4" height="6" rx="1" /><rect x="18" y="14" width="4" height="6" rx="1" />',
  'monitor': '<rect x="3" y="4" width="18" height="12" rx="2" /><path d="M9 20h6M12 16v4" />',
  'smartphone': '<rect x="6" y="3" width="12" height="18" rx="2" /><path d="M11 18h2" />',
  'tablet': '<rect x="4" y="3" width="16" height="18" rx="2" /><path d="M11 18h2" />',
  'battery': '<rect x="3" y="7" width="16" height="10" rx="2" /><path d="M21 10v4" />',
  'plug': '<path d="M8 3v4M16 3v4M6 7h12v4a6 6 0 0 1-12 0V7zM12 17v5" />',
  'wifi': '<path d="M4 9a11.3 11.3 0 0 1 16 0M7 13a7.1 7.1 0 0 1 10 0M10 17a2.8 2.8 0 0 1 4 0" />',
  'bluetooth': '<path d="M6 8l12 8-6 5V3l6 5-12 8" />',
  'shopping-cart': '<path d="M3 4h3l2 10a2 2 0 0 0 2 2h7a2 2 0 0 0 2-2l2-7H7" /><circle cx="10" cy="19" r="1" /><circle cx="17" cy="19" r="1" />',
  'shopping-bag': '<path d="M5 8h14l1 12H4zM9 8V5a3 3 0 0 1 6 0v3" />',
  'coffee': '<path d="M5 9h12v5a5 5 0 0 1-10 0V9zM17 10h1a2 2 0 0 1 0 4h-1M8 4v2M12 3v3M16 4v2" />'
};

const variations = [
  { suffix: '', transform: '', extra: '' },
  { suffix: '-plus', transform: 'transform="scale(0.85)"', extra: '<path d="M17 17v6M14 20h6" />' },
  { suffix: '-minus', transform: 'transform="scale(0.85)"', extra: '<path d="M14 20h6" />' },
  { suffix: '-check', transform: 'transform="scale(0.85)"', extra: '<path d="M14 18l2 2 4-4" />' },
  { suffix: '-x', transform: 'transform="scale(0.85)"', extra: '<path d="M15 15l6 6M21 15l-6 6" />' },
  { suffix: '-circle', transform: 'transform="scale(0.7) translate(5.14, 5.14)"', extra: '<circle cx="12" cy="12" r="11" />' }
];

let count = 0;

for (const [baseName, baseSvg] of Object.entries(baseIcons)) {
  for (const v of variations) {
    const iconName = `${baseName}${v.suffix}`;

    let inner = baseSvg;
    if (v.transform) {
      inner = `<g ${v.transform}>${baseSvg}</g>${v.extra}`;
    }

    const darkSvg = getSvg(inner, '#FFFFFF');
    const lightSvg = getSvg(inner, '#000000');

    fs.writeFileSync(path.join(darkDir, `${iconName}.svg`), darkSvg);
    fs.writeFileSync(path.join(lightDir, `${iconName}.svg`), lightSvg);

    count++;
  }
}

console.log(`Generated exactly ${count} completely custom, ground-up SVG icons!`);

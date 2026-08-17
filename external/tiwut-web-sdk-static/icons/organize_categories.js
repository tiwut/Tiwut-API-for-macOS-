const fs = require('fs');
const path = require('path');

const categories = [
  { name: 'arrows', keywords: ['arrow', 'chevron', 'move', 'sort', 'point'] },
  { name: 'devices', keywords: ['computer', 'laptop', 'phone', 'monitor', 'cpu', 'gpu', 'server', 'database', 'hard-drive', 'tablet', 'memory', 'microchip', 'terminal'] },
  { name: 'media', keywords: ['music', 'video', 'camera', 'image', 'volume', 'play', 'pause', 'film', 'headphone', 'speaker', 'mic'] },
  { name: 'files', keywords: ['file', 'folder', 'archive', 'document', 'paper'] },
  { name: 'communication', keywords: ['mail', 'message', 'user', 'users', 'chat', 'contact'] },
  { name: 'nature', keywords: ['tree', 'leaf', 'plant', 'sun', 'moon', 'cloud', 'weather', 'lightning', 'snow', 'rain', 'branch'] },
  { name: 'commerce', keywords: ['shopping', 'cart', 'bag', 'credit', 'coin', 'dollar', 'euro'] },
  { name: 'ui', keywords: ['menu', 'search', 'setting', 'trash', 'edit', 'save', 'check', 'close', 'lock', 'unlock', 'eye', 'bell', 'shield', 'link', 'heart', 'star', 'battery', 'plug', 'wifi', 'bluetooth', 'coffee', 'plus', 'minus', 'x'] }
];

const colorFolders = [
  'darkmode', 'lightmode', 'red', 'green', 'blue', 'orange', 'pink', 'gold', 
  'yellow', 'gray', 'light-blue', 'light-red', 'light-green', 'light-pink'
];

let moved = 0;

for (const folder of colorFolders) {
  const dirPath = path.join(__dirname, folder);
  if (!fs.existsSync(dirPath)) continue;

  const files = fs.readdirSync(dirPath, { withFileTypes: true });

  for (const file of files) {
    if (!file.isFile() || !file.name.endsWith('.svg')) continue;

    const baseName = path.parse(file.name).name.toLowerCase();
    
    let targetCategory = 'others';
    for (const cat of categories) {
      if (cat.keywords.some(kw => baseName.includes(kw))) {
        targetCategory = cat.name;
        break;
      }
    }

    const catDirPath = path.join(dirPath, targetCategory);
    if (!fs.existsSync(catDirPath)) {
      fs.mkdirSync(catDirPath, { recursive: true });
    }

    const oldPath = path.join(dirPath, file.name);
    const newPath = path.join(catDirPath, file.name);
    
    fs.renameSync(oldPath, newPath);
    moved++;
  }
}

console.log(`Organized ${moved} icons into categories across all folders!`);

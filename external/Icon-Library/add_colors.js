const fs = require('fs');
const path = require('path');

const colors = {
  'red': 'red',
  'green': 'green',
  'blue': 'blue',
  'orange': 'orange',
  'pink': 'pink',
  'gold': 'gold',
  'yellow': 'yellow',
  'gray': 'gray',
  'light-blue': 'lightblue',
  'light-red': '#ff6666',
  'light-green': 'lightgreen',
  'light-pink': 'lightpink'
};

const srcDir = path.join(__dirname, 'lightmode');
const baseColor = '#000000';

let generated = 0;

if (fs.existsSync(srcDir)) {
  const files = fs.readdirSync(srcDir).filter(f => f.endsWith('.svg'));
  
  for (const [colorName, colorValue] of Object.entries(colors)) {
    const destDir = path.join(__dirname, colorName);
    if (!fs.existsSync(destDir)) {
      fs.mkdirSync(destDir, { recursive: true });
    }
    
    for (const file of files) {
      const srcPath = path.join(srcDir, file);
      const destPath = path.join(destDir, file);
      
      const content = fs.readFileSync(srcPath, 'utf-8');
      const newContent = content.replace(`stroke="${baseColor}"`, `stroke="${colorValue}"`);
      
      fs.writeFileSync(destPath, newContent);
      generated++;
    }
  }
}

console.log(`Generated ${generated} colored SVGs in extra color folders!`);

const fs = require('fs');
const path = require('path');

const colors = [
  'red', 'green', 'blue', 'orange', 'pink', 'gold', 'yellow', 'gray',
  'light-blue', 'light-red', 'light-green', 'light-pink'
];

const dirs = ['darkmode', 'lightmode'];
let removed = 0;

for (const dir of dirs) {
  const dirPath = path.join(__dirname, dir);
  if (!fs.existsSync(dirPath)) continue;

  const files = fs.readdirSync(dirPath);
  for (const file of files) {
    if (colors.some(c => file.endsWith(`-${c}.svg`))) {
      fs.unlinkSync(path.join(dirPath, file));
      removed++;
    }
  }
}

console.log(`Removed ${removed} files`);

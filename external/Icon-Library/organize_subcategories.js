const fs = require('fs');
const path = require('path');

const colorFolders = [
  'darkmode', 'lightmode', 'red', 'green', 'blue', 'orange', 'pink', 'gold', 
  'yellow', 'gray', 'light-blue', 'light-red', 'light-green', 'light-pink'
];

let moved = 0;

for (const colorFolder of colorFolders) {
  const colorPath = path.join(__dirname, colorFolder);
  if (!fs.existsSync(colorPath)) continue;

  const categories = fs.readdirSync(colorPath, { withFileTypes: true })
                       .filter(dirent => dirent.isDirectory())
                       .map(dirent => dirent.name);

  for (const category of categories) {
    const categoryPath = path.join(colorPath, category);
    const files = fs.readdirSync(categoryPath, { withFileTypes: true })
                    .filter(dirent => dirent.isFile() && dirent.name.endsWith('.svg'))
                    .map(dirent => dirent.name);

    // Group by first word before hyphen
    const groups = {};
    for (const file of files) {
      const base = file.split('-')[0].replace('.svg', '');
      if (!groups[base]) groups[base] = [];
      groups[base].push(file);
    }

    // Move groups with 3 or more files into a subfolder
    for (const [base, list] of Object.entries(groups)) {
      if (list.length >= 3) {
        const subCatPath = path.join(categoryPath, base);
        if (!fs.existsSync(subCatPath)) {
          fs.mkdirSync(subCatPath, { recursive: true });
        }

        for (const file of list) {
          const oldPath = path.join(categoryPath, file);
          const newPath = path.join(subCatPath, file);
          fs.renameSync(oldPath, newPath);
          moved++;
        }
      }
    }
  }
}

console.log(`Organized ${moved} icons into deeper sub-categories across all folders!`);

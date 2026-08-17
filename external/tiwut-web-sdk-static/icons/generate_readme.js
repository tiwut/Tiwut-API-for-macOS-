const fs = require('fs');
const path = require('path');

const srcDir = path.join(__dirname, 'lightmode');
if (!fs.existsSync(srcDir)) {
  console.error("lightmode directory not found");
  process.exit(1);
}

function getFilesRecursively(dir) {
  let results = [];
  const list = fs.readdirSync(dir, { withFileTypes: true });
  list.forEach(file => {
    const fullPath = path.join(dir, file.name);
    if (file.isDirectory()) {
      results = results.concat(getFilesRecursively(fullPath));
    } else if (file.name.endsWith('.svg')) {
      results.push(path.relative(srcDir, fullPath).replace(/\\/g, '/'));
    }
  });
  return results;
}

const files = getFilesRecursively(srcDir);
files.sort((a, b) => a.localeCompare(b));

const columns = [
  { name: 'Name', path: '' },
  { name: 'Dark Mode', path: '../darkmode' },
  { name: 'Light Mode', path: '../lightmode' },
  { name: 'Red', path: '../red' },
  { name: 'Green', path: '../green' },
  { name: 'Blue', path: '../blue' },
  { name: 'Orange', path: '../orange' },
  { name: 'Pink', path: '../pink' },
  { name: 'Gold', path: '../gold' },
  { name: 'Yellow', path: '../yellow' },
  { name: 'Gray', path: '../gray' },
  { name: 'Light Blue', path: '../light-blue' },
  { name: 'Light Red', path: '../light-red' },
  { name: 'Light Green', path: '../light-green' },
  { name: 'Light Pink', path: '../light-pink' }
];

const itemsPerPage = 20;
const totalPages = Math.ceil(files.length / itemsPerPage);
const outDir = path.join(__dirname, 'ALL_ICONS');

if (fs.existsSync(path.join(__dirname, 'ALL_ICONS.md'))) {
  fs.unlinkSync(path.join(__dirname, 'ALL_ICONS.md'));
}

if (!fs.existsSync(outDir)) {
  fs.mkdirSync(outDir, { recursive: true });
}

function getPaginationLinks(currentPage, totalPages) {
  let links = [];
  if (currentPage > 1) {
    links.push(`[<< Previous](ALL_ICONS_${currentPage - 1}.md)`);
  }
  links.push(`Page ${currentPage} of ${totalPages}`);
  if (currentPage < totalPages) {
    links.push(`[Next >>](ALL_ICONS_${currentPage + 1}.md)`);
  }
  return links.join(' | ');
}

for (let i = 0; i < totalPages; i++) {
  const pageNumber = i + 1;
  const chunk = files.slice(i * itemsPerPage, (i + 1) * itemsPerPage);

  let md = `# Full Icon Catalog - Page ${pageNumber}\n\n`;

  const nav = getPaginationLinks(pageNumber, totalPages);
  md += `${nav}\n\n`;

  md += '| ' + columns.map(c => c.name).join(' | ') + ' |\n';
  md += '|' + columns.map(c => c.name === 'Name' ? ' :--- ' : ' :---: ').join('|') + '|\n';

  for (const file of chunk) {
    const baseName = path.parse(file).name;
    let row = `| \`${baseName}\` |`;
    for (let j = 1; j < columns.length; j++) {
      const col = columns[j];
      row += ` ![${baseName}](${col.path}/${file}) |`;
    }
    md += row + '\n';
  }

  md += `\n${nav}\n`;

  fs.writeFileSync(path.join(outDir, `ALL_ICONS_${pageNumber}.md`), md);
}

console.log(`Successfully generated paginated ALL_ICONS files in ALL_ICONS folder!`);

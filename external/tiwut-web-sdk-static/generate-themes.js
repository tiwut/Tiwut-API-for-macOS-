const fs = require('fs');
const path = require('path');

const generateColors = (baseHue, isDark, variant) => {
  const lightnessPrimary = isDark ? 10 : 98;
  const lightnessSecondary = isDark ? 15 : 95;
  const textPrimary = isDark ? 95 : 10;
  const textSecondary = isDark ? 70 : 40;
  const accentLightness = isDark ? 60 : 40;

  let saturation = 50;
  let radius = '8px';
  let font = 'system-ui, -apple-system, sans-serif';
  let glassBg = isDark ? 'rgba(0, 0, 0, 0.4)' : 'rgba(255, 255, 255, 0.4)';
  let glassBorder = isDark ? 'rgba(255, 255, 255, 0.1)' : 'rgba(0, 0, 0, 0.1)';

  if (variant === 'clean') {
    saturation = 20;
    radius = '0px';
  } else if (variant === 'old') {
    saturation = 30;
    radius = '2px';
    font = '"Times New Roman", Times, serif';
    glassBg = isDark ? 'rgba(50, 50, 50, 0.8)' : 'rgba(230, 230, 230, 0.8)';
  } else if (variant === 'rounded') {
    radius = '24px';
  } else if (variant === 'neon') {
    saturation = 100;
  }

  return `
    --bg-primary: hsl(${baseHue}, ${saturation}%, ${lightnessPrimary}%);
    --bg-secondary: hsl(${baseHue}, ${saturation}%, ${lightnessSecondary}%);
    --text-primary: hsl(${baseHue}, ${saturation}%, ${textPrimary}%);
    --text-secondary: hsl(${baseHue}, ${saturation}%, ${textSecondary}%);
    --accent-color: hsl(${baseHue}, ${saturation}%, ${accentLightness}%);
    --accent-hover: hsl(${baseHue}, ${saturation}%, ${accentLightness > 50 ? accentLightness - 10 : accentLightness + 10}%);
    --border-color: hsl(${baseHue}, ${saturation}%, ${isDark ? 25 : 85}%);
    --border-radius: ${radius};
    --font-family: ${font};
    --glass-bg: ${glassBg};
    --glass-border: ${glassBorder};
    --glass-blur: ${variant === 'old' ? '0px' : '12px'};
`;
};

const palettes = [
  { name: 'gray', hue: 0 },
  { name: 'blue', hue: 210 },
  { name: 'green', hue: 140 },
  { name: 'purple', hue: 270 },
  { name: 'orange', hue: 30 }
];

const variants = ['modern', 'clean', 'old', 'rounded', 'neon'];

let darkCss = '\n';
let lightCss = '\n';

palettes.forEach(palette => {
  variants.forEach(variant => {
    const themeName = `${palette.name}-${variant}`;
    
    darkCss += `\n[data-theme="theme-dark-${themeName}"] {\n`;
    darkCss += generateColors(palette.hue, true, variant);
    darkCss += `}\n`;

    lightCss += `\n[data-theme="theme-light-${themeName}"] {\n`;
    darkCss += generateColors(palette.hue, false, variant);
    lightCss += generateColors(palette.hue, false, variant);
    lightCss += `}\n`;
  });
});

fs.writeFileSync(path.join(__dirname, 'themes', 'dark.css'), darkCss);
fs.writeFileSync(path.join(__dirname, 'themes', 'light.css'), lightCss);
console.log('Themes generated!');

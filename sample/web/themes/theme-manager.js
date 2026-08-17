export class ThemeManager {
  constructor() {
    this.root = document.documentElement;
    this.currentTheme = this.root.dataset.theme || 'theme-dark-modern';
    this.init();
  }

  init() {
    this.setTheme(this.currentTheme);

    window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', e => {
      if (!this.root.dataset.theme) {
        this.setTheme(e.matches ? 'theme-dark-modern' : 'theme-light-modern');
      }
    });
  }

  setTheme(themeName) {
    this.currentTheme = themeName;
    this.root.dataset.theme = themeName;
    localStorage.setItem('tiwut-web-sdk-theme', themeName);
    
    window.dispatchEvent(new CustomEvent('themeChanged', { detail: { theme: themeName } }));
  }

  getAvailableThemes() {
    return [
    ];
  }
}

if (typeof window !== 'undefined') {
  const savedTheme = localStorage.getItem('tiwut-web-sdk-theme');
  if (savedTheme) {
    document.documentElement.dataset.theme = savedTheme;
  }
  window.tiwutThemeManager = new ThemeManager();
}

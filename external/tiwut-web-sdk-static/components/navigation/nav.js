export class NavigationManager {
  constructor(navElementId = 't-main-nav', toggleId = 't-nav-toggle') {
    this.navElement = document.getElementById(navElementId);
    this.toggleBtn = document.getElementById(toggleId);
    
    if (this.navElement && this.toggleBtn) {
      this.init();
    }
  }

  init() {
    this.toggleBtn.addEventListener('click', () => {
      const links = this.navElement.querySelector('.t-nav-links');
      if (links) {
        links.classList.toggle('active');
      }
    });
  }
}

if (typeof window !== 'undefined') {
  window.addEventListener('DOMContentLoaded', () => {
    window.tiwutNav = new NavigationManager();
  });
}

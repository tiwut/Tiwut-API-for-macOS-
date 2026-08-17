# Navigation

Responsive navigation bars.

## Usage
Include the CSS and JS:
```html
<link rel="stylesheet" href="path/to/components/navigation/nav.css">
<script type="module" src="path/to/components/navigation/nav.js"></script>
```

Example HTML:
```html
<nav id="t-main-nav" class="t-nav t-nav-glass">
  <div class="t-nav-brand">
    <!-- SVG Icon Integration -->
    <img src="../../icons/darkmode/ui/home.svg" alt="Logo" class="t-nav-icon">
    Tiwut SDK
  </div>
  
  <button id="t-nav-toggle" class="t-nav-toggle">
    <img src="../../icons/darkmode/ui/menu.svg" alt="Menu" class="t-nav-icon">
  </button>
  
  <div class="t-nav-links">
    <a href="#" class="t-nav-link active">Home</a>
    <a href="#" class="t-nav-link">About</a>
    <a href="#" class="t-nav-link">Contact</a>
  </div>
</nav>
```

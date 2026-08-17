# Themes

The robust theme engine providing ~50 variations.

## Overview
Themes are structured around a set of CSS Custom Properties (variables) applied to `.theme-*` classes. 

## Files
- `dark.css`: Contains 25 dark theme variations.
- `light.css`: Contains 25 light theme variations.
- `theme-manager.js`: A utility script to dynamically switch themes by updating the `data-theme` attribute on the document root.

## Usage
Include the theme files and apply a class or data attribute to your root element (`<html>` or `<body>`).

```html
<link rel="stylesheet" href="path/to/themes/dark.css">
<link rel="stylesheet" href="path/to/themes/light.css">
<script type="module" src="path/to/themes/theme-manager.js"></script>
```

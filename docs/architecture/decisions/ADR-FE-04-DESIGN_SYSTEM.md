# ADR-FE-04: Design System & Tokens Architecture

**Status:** Accepted  
**Date:** 2025-01-27  
**Team:** Frontend  
**Type:** Architecture Decision Record

## Context

OHT-50 cần một design system nhất quán cho:
- Color tokens (OKLCH color space)
- Typography system
- Spacing và layout tokens
- Component states (Idle/Move/Dock/Fault/E-Stop)
- Dark mode support
- Accessibility compliance

Cần đảm bảo consistency, maintainability và accessibility.

## Decision

**Sử dụng CSS Custom Properties với OKLCH color space:**

```css
/* Design tokens structure */
:root {
  /* Color tokens - OKLCH space */
  --primary: oklch(0.6489 0.2370 26.9728);
  --secondary: oklch(0.9680 0.2110 109.7692);
  --accent: oklch(0.5635 0.2408 260.8178);
  --destructive: oklch(0.5770 0.2450 27.3250);
  
  /* System states */
  --state-idle: oklch(0.7323 0.2492 142.4953);
  --state-move: oklch(0.6489 0.2370 26.9728);
  --state-dock: oklch(0.9680 0.2110 109.7692);
  --state-fault: oklch(0.5770 0.2450 27.3250);
  --state-estop: oklch(0.5931 0.2726 328.3634);
  
  /* Typography */
  --font-sans: 'Inter', system-ui, sans-serif;
  --font-mono: 'JetBrains Mono', monospace;
  --font-serif: 'Merriweather', serif;
  
  /* Spacing */
  --spacing-xs: 0.25rem;
  --spacing-sm: 0.5rem;
  --spacing-md: 1rem;
  --spacing-lg: 1.5rem;
  --spacing-xl: 2rem;
  
  /* Border radius */
  --radius-sm: 0.25rem;
  --radius-md: 0.5rem;
  --radius-lg: 0.75rem;
  
  /* Shadows */
  --shadow-sm: 0 1px 2px 0 rgb(0 0 0 / 0.05);
  --shadow-md: 0 4px 6px -1px rgb(0 0 0 / 0.1);
  --shadow-lg: 0 10px 15px -3px rgb(0 0 0 / 0.1);
}

/* Dark mode tokens */
[data-theme="dark"] {
  --background: oklch(0.18 0.03 50);
  --foreground: oklch(0.92 0.02 85);
  --card: oklch(0.22 0.03 50);
  --border: oklch(0.30 0.02 50);
}
```

**Component architecture:**
- Atomic design principles (atoms, molecules, organisms)
- CSS-in-JS cho dynamic styling
- Design tokens cho consistency
- Accessibility-first approach

## Consequences

### Positive:
- ✅ **Consistency:** Unified design language
- ✅ **Maintainability:** Centralized design tokens
- ✅ **Accessibility:** WCAG 2.1 compliance
- ✅ **Performance:** CSS custom properties optimization
- ✅ **Dark Mode:** Seamless theme switching

### Negative:
- ❌ **Browser Support:** CSS custom properties (IE11+)
- ❌ **Complexity:** Token management overhead
- ❌ **Learning Curve:** OKLCH color space

## Alternatives Considered

1. **Tailwind CSS với custom config**
   - Pros: Utility-first, rapid development
   - Cons: Bundle size, less semantic

2. **Styled Components**
   - Pros: CSS-in-JS, dynamic styling
   - Cons: Runtime overhead, bundle size

3. **CSS Modules**
   - Pros: Scoped styles, no runtime cost
   - Cons: Less dynamic, manual token management

## Implementation Notes

- Use PostCSS cho token processing
- Implement color contrast validation
- Generate design tokens từ Figma/Sketch
- Document component usage guidelines

## Accessibility Requirements

- **Color contrast:** WCAG AA (4.5:1)
- **Focus indicators:** Visible focus rings
- **Keyboard navigation:** Full keyboard support
- **Screen readers:** Semantic HTML structure

## Component States

1. **Idle:** Green - System ready
2. **Move:** Blue - In motion
3. **Dock:** Yellow - Docking operation
4. **Fault:** Red - Error condition
5. **E-Stop:** Red - Emergency stop

## Related Documents

- [Design System Documentation](../design/DESIGN_SYSTEM.md)
- [Component Library](../design/COMPONENT_LIBRARY.md)
- [Accessibility Guidelines](../design/ACCESSIBILITY.md)
# ADR-FE-05: Internationalization (i18n) Strategy

**Status:** Accepted  
**Date:** 2025-01-27  
**Team:** Frontend  
**Type:** Architecture Decision Record

## Context

OHT-50 cần hỗ trợ đa ngôn ngữ:
- Tiếng Việt (primary)
- Tiếng Anh (secondary)
- Có thể mở rộng cho các ngôn ngữ khác
- Technical terms và user interface
- Date/time formatting
- Number formatting

Cần đảm bảo maintainability và performance.

## Decision

**Sử dụng react-i18next với TypeScript support:**

```javascript
// i18n configuration
import i18n from 'i18next';
import { initReactI18next } from 'react-i18next';

i18n
  .use(initReactI18next)
  .init({
    resources: {
      vi: {
        translation: {
          // Navigation
          'nav.overview': 'Tổng quan',
          'nav.vaxCharts': 'Đồ thị v‑a‑x',
          'nav.logs': 'Nhật ký',
          'nav.safetyConfig': 'Cấu hình an toàn',
          
          // System states
          'state.idle': 'Idle',
          'state.move': 'Move',
          'state.dock': 'Dock',
          'state.fault': 'Fault',
          'state.estop': 'E‑Stop',
          
          // Technical terms
          'velocity': 'Vận tốc',
          'acceleration': 'Gia tốc',
          'position': 'Vị trí',
          'emergencyStop': 'Dừng khẩn cấp'
        }
      },
      en: {
        translation: {
          'nav.overview': 'Overview',
          'nav.vaxCharts': 'VAX Charts',
          'nav.logs': 'Logs',
          'nav.safetyConfig': 'Safety Config',
          
          'state.idle': 'Idle',
          'state.move': 'Move',
          'state.dock': 'Dock',
          'state.fault': 'Fault',
          'state.estop': 'E‑Stop',
          
          'velocity': 'Velocity',
          'acceleration': 'Acceleration',
          'position': 'Position',
          'emergencyStop': 'Emergency Stop'
        }
      }
    },
    lng: 'vi', // default language
    fallbackLng: 'en',
    interpolation: {
      escapeValue: false
    }
  });

// Usage in components
const { t } = useTranslation();

return (
  <div>
    <h1>{t('nav.overview')}</h1>
    <span className="badge badge-idle">{t('state.idle')}</span>
  </div>
);
```

**Translation strategy:**
- Namespace-based organization
- Type-safe translations với TypeScript
- Lazy loading cho language packs
- Fallback mechanism

## Consequences

### Positive:
- ✅ **Maintainability:** Centralized translations
- ✅ **Type Safety:** TypeScript integration
- ✅ **Performance:** Lazy loading support
- ✅ **Scalability:** Easy to add new languages
- ✅ **Developer Experience:** Good tooling support

### Negative:
- ❌ **Bundle Size:** Additional i18n library
- ❌ **Complexity:** Translation management
- ❌ **Testing:** Need to test multiple languages

## Alternatives Considered

1. **Native browser Intl API**
   - Pros: No dependencies, built-in
   - Cons: Limited features, manual implementation

2. **Custom translation system**
   - Pros: Full control, minimal bundle
   - Cons: Development overhead, maintenance

3. **Next.js i18n**
   - Pros: Framework integration
   - Cons: Next.js specific, overkill

## Implementation Notes

- Use translation keys cho technical terms
- Implement pluralization rules
- Add date/time formatting
- Support RTL languages (future)

## Translation Guidelines

1. **Keys:** Use dot notation (e.g., `nav.overview`)
2. **Context:** Provide context cho translators
3. **Variables:** Use interpolation cho dynamic content
4. **Plurals:** Handle plural forms correctly
5. **Technical terms:** Maintain consistency

## Language Support

### Phase 1 (Current):
- Vietnamese (vi) - Primary
- English (en) - Secondary

### Phase 2 (Future):
- Chinese (zh)
- Japanese (ja)
- Korean (ko)

## Related Documents

- [Translation Guidelines](../design/TRANSLATION_GUIDELINES.md)
- [Technical Terms Dictionary](../design/TECHNICAL_TERMS.md)
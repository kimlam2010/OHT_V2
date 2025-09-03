import antfu from "@antfu/eslint-config";

export default antfu({
  react: true,
  typescript: true,
  stylistic: {
    indent: 2,
    quotes: "single",
  },
  ignores: [
    "dist",
    "node_modules",
    "*.d.ts",
    "*.gen.ts",
    "**/*.gen.ts",
    "*.md",
    "package.json",
    "package-lock.json",
    "tsconfig.json",
    "eslint.config.js",
    "components.json",
    ".cta.json",
    ".github",
  ],
  rules: {
    "react-refresh/only-export-components": "off",
    "symbol-description": 0,
    "no-console": "warn",
    "no-alert": "warn",
    "no-tabs": "warn",
    "ts/no-explicit-any": "warn",
    "unused-imports/no-unused-vars": "warn",
    "import/first": 0,
    "node/prefer-global/process": 0,
    "style/no-tabs": 0,
    "unicorn/no-new-array": 0,
  },
});

/** @type {import('tailwindcss').Config} */
export default {
  content: [
    './index.html',
    './src/**/*.{ts,tsx}',
    '../.superdesign/design_iterations/**/*.html'
  ],
  theme: { extend: {} },
  plugins: [require('flowbite/plugin')]
}



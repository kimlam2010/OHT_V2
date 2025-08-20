#!/usr/bin/env node
import express from 'express';
import compression from 'compression';
import serveStatic from 'serve-static';
import path from 'path';
import { fileURLToPath } from 'url';
import minimist from 'minimist';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const argv = minimist(process.argv.slice(2));
const port = Number(argv.port) || Number(process.env.PORT) || 8081;
const distDir = path.resolve(__dirname, 'dist');

const app = express();
app.disable('x-powered-by');
app.use(compression());
app.use(serveStatic(distDir, {
  index: ['index.html'],
  setHeaders(res, filePath) {
    if (/\.(css|js|woff2?|ttf|otf|eot|svg|png|jpe?g|gif|ico)$/i.test(filePath)) {
      res.setHeader('Cache-Control', 'public, max-age=604800');
    }
  }
}));

app.get('*', (req, res) => {
  res.sendFile(path.join(distDir, 'index.html'));
});

app.listen(port, () => {
  console.log(`[oht50-frontend] listening on http://0.0.0.0:${port}`);
});



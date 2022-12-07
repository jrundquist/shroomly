import { Config } from "./config";
import { Stats } from "./types";

/**
 * This HTML file acts as a template that we insert all our generated
 * application code into before sending it to the client as regular HTML.
 * Note we're returning a template string from this function.
 */
const html = ({
  stats,
  content,
  config,
  css = "",
}: {
  stats: Stats;
  content: string;
  config: Config;
  css?: string;
}): string => `<!DOCTYPE html>
  <html lang="en">
    <head>
      <meta charset="utf-8" />
      <meta name="viewport" content="minimum-scale=1, initial-scale=1, width=device-width" />
      <link rel="apple-touch-icon" sizes="180x180" href="${config.app.PUBLIC_URL}/apple-touch-icon.png" />
      <link rel="icon" type="image/png" sizes="32x32" href="${config.app.PUBLIC_URL}/favicon-32x32.png" />
      <link rel="icon" type="image/png" sizes="16x16" href="${config.app.PUBLIC_URL}/favicon-16x16.png" />
      <link rel="manifest" href="${config.app.PUBLIC_URL}/manifest.json" />
      <link rel="mask-icon" href="${config.app.PUBLIC_URL}/safari-pinned-tab.svg" color="#5bbad5" />
      <meta name="msapplication-TileColor" content="#9a6d2f" />
      <meta name="theme-color" content="${config.app.THEME_COLOR}" />
    
      <title>${config.app.TITLE}</title>

      ${stats.styles
        .map((filename) => `<link rel="stylesheet" href="${config.app.DIST_URL}/${filename}" />`)
        .join("\n")}
      <style id="jss-server-side">${css}</style>
      <script id="config-server-side">
        window.__CONFIG__ = ${JSON.stringify(config)};
      </script>
    </head>
    <body>
      <div id="root">${content}</div>
      ${stats.scripts
        .map((filename) => `<script src="${config.app.DIST_URL}/${filename}" crossorigin></script>`)
        .join("\n")}
    </body>
  </html>`;

export default html;

#ifndef RATTIP_INIT_HPP
#define RATTIP_INIT_HPP

#include <filesystem>
#include <fstream>
#include "error.h"

static const char *PAGE_HTML = R"(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>{title}</title>
  <link rel="stylesheet" href="/styles/{css}">
</head>

<body>
{navbar}
{md_content}
<footer class="rattip-footer"> made with ❤️ by rattip</footer>
</body>

</html>
)";

static const char *BLOG_HTML = R"(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>{title}</title>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.11.1/styles/sunburst.min.css">
  <link rel="stylesheet" href="/styles/{css}">
</head>

<body>
  <header class="rattip-header">
    <span class="rattip-date">{blog_date}</span>
  </header>

{md_content}

  <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.9.0/highlight.min.js"></script>
  <script>hljs.highlightAll();</script>

<footer class="rattip-footer"> made with ❤️ by rattip</footer>
</body>

</html>
)";

static const char *GLOBAL_CSS = R"(
@import url('https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,400;0,700;1,400&family=JetBrains+Mono:wght@400;500&family=Source+Serif+4:ital,opsz,wght@0,8..60,300;0,8..60,400;1,8..60,300&display=swap');

:root {
  --bg: #0a0a0a;
  --bg-raised: #111111;
  --bg-code: #161616;
  --border: #222222;
  --border-dim: #1a1a1a;
  --text: #d4cfc8;
  --text-dim: #6b6560;
  --text-muted: #3d3a37;
  --accent: #c9a96e;
  --accent-dim: #8a6f3e;
  --white: #f0ebe4;
}

* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

::selection {
  background: var(--accent);
  color: var(--bg);
}

html {
  scroll-behavior: smooth;
}

body {
  background-color: var(--bg);
  color: var(--text);
  font-family: 'Source Serif 4', Georgia, serif;
  font-size: 0.80rem;
  font-weight: 300;
  max-width: 700px;
  margin: 0 auto;
  padding: 3rem 1.5rem 6rem;
  line-height: 1.85;
  letter-spacing: 0.01em;
}

.rattip-header {
  display: flex;
  justify-content: space-between;
  align-items: baseline;
  padding-bottom: 1.5rem;
  margin-bottom: 3rem;
  border-bottom: 1px solid var(--border);
  position: relative;
}

.rattip-header::after {
  content: '';
  position: absolute;
  bottom: -3px;
  left: 0;
  width: 3rem;
  height: 1px;
  background: var(--accent);
}

.rattip-date {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.72rem;
  color: var(--text-dim);
  letter-spacing: 0.12em;
  text-transform: uppercase;
}

.rattip-footer {
  margin-top: 5rem;
  padding-top: 1.5rem;
  border-top: 1px solid var(--border);
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.7rem;
  color: var(--text-muted);
  letter-spacing: 0.1em;
  text-transform: uppercase;
  text-align: center;
}

.rattip-h1 {
  font-family: 'Playfair Display', serif;
  font-size: 2.2rem;
  font-weight: 700;
  color: var(--white);
  line-height: 1.2;
  margin: 2.5rem 0 1.2rem;
  letter-spacing: -0.02em;
}

.rattip-h2 {
  font-family: 'Playfair Display', serif;
  font-size: 1.6rem;
  font-weight: 400;
  color: var(--white);
  margin: 2.5rem 0 0.9rem;
  padding-left: 1rem;
  border-left: 2px solid var(--accent);
  line-height: 1.3;
}

.rattip-h3 {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.82rem;
  font-weight: 500;
  color: var(--accent);
  text-transform: uppercase;
  letter-spacing: 0.15em;
  margin: 2rem 0 0.7rem;
}

.rattip-h4,
.rattip-h5,
.rattip-h6 {
  font-family: 'Source Serif 4', serif;
  font-size: 1rem;
  font-weight: 400;
  color: var(--text);
  margin: 1.5rem 0 0.5rem;
}

.rattip-p {
  margin: 1.1rem 0;
  color: var(--text);
}

.rattip-a {
  color: var(--accent);
  text-decoration: none;
  border-bottom: 1px solid var(--accent-dim);
  transition: border-color 0.2s, color 0.2s;
}

.rattip-a:hover {
  color: var(--white);
  border-bottom-color: var(--white);
}

.rattip-ul {
  margin: 1rem 0;
  padding-left: 0;
  list-style: none;
}

.rattip-ol {
  margin: 1rem 0 1rem 1.5rem;
}

.rattip-ul .rattip-li {
  padding-left: 1.2rem;
  position: relative;
  margin: 0.4rem 0;
}

.rattip-ul .rattip-li::before {
  content: '—';
  position: absolute;
  left: 0;
  color: var(--accent);
  font-size: 0.7em;
  top: 0.15em;
}

.rattip-ol .rattip-li {
  margin: 0.4rem 0;
}

.rattip-li input[type="checkbox"] {
  margin-right: 0.5rem;
  accent-color: var(--accent);
  cursor: default;
}

.rattip-code {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.62em;
  background-color: var(--bg-code);
  color: var(--accent);
  padding: 0.0.15rem 0.45rem;
  border-radius: 2px;
  border: 1px solid var(--border);
}

.rattip-pre {
  background-color: var(--bg-code);
  border: 1px solid var(--border);
  border-left: 1px solid var(--accent);
  padding: 0.4rem 1.0rem;
  border-radius: 4px;
  overflow-x: auto;
  margin: 1.8rem 0;
}

.rattip-pre .rattip-code {
  background: none;
  border: none;
  padding: 0;
  color: var(--text);
  font-size: 0.7em;
  line-height: 1.7;
}

.rattip-blockquote {
  margin: 0.5rem 0;
  padding: 0.2rem 0.6rem;
  background: none;
  border-left: 1px solid var(--accent);
  color: var(--text-dim);
  font-size: 0.88em;
  line-height: 1.5;
}

.rattip-table {
  width: 100%;
  border-collapse: collapse;
  margin: 2rem 0;
  font-size: 0.72rem;
}

.rattip-thead {
  border-bottom: 2px solid var(--accent-dim);
}

.rattip-th {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.62rem;
  text-transform: uppercase;
  letter-spacing: 0.1em;
  color: var(--accent);
  padding: 0.7rem 1rem;
  text-align: left;
  font-weight: 500;
}

.rattip-td {
  padding: 0.65rem 1rem;
  border-bottom: 1px solid var(--border-dim);
  color: var(--text);
}

.rattip-tr:hover .rattip-td {
  background-color: var(--bg-raised);
}

.rattip-b {
  font-weight: 700;
  color: var(--white);
}

.rattip-i {
  font-style: italic;
}

.rattip-del {
  text-decoration: line-through;
  color: var(--text-muted);
}

.rattip-u {
  text-decoration: underline;
  text-underline-offset: 3px;
}

.rattip-hr {
  border: none;
  height: 1px;
  margin: 3rem 0;
  background: linear-gradient(to right, transparent, var(--border), transparent);
}

.rattip-img {
  max-width: 100%;
  border-radius: 4px;
  margin: 2rem 0;
  border: 1px solid var(--border);
  display: block;
}

.rattip-sup,
.rattip-sub {
  font-size: 0.62em;
  font-family: 'JetBrains Mono', monospace;
  color: var(--accent);
}

.rattip-nav {
  display: flex;
  gap: 2rem;
  padding-bottom: 1.5rem;
  margin-bottom: 2.5rem;
  border-bottom: 1px solid var(--border);
  position: relative;
}

.rattip-nav::after {
  content: '';
  position: absolute;
  bottom: -3px;
  left: 0;
  width: 3rem;
  height: 1px;
  background: var(--accent);
}

.rattip-nav-a {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.72rem;
  text-transform: uppercase;
  letter-spacing: 0.12em;
  color: var(--text-dim);
  text-decoration: none;
  border-bottom: none;
  transition: color 0.2s;
}

.rattip-nav-a:hover {
  color: var(--accent);
}

.rattip-spoiler {
  background-color: var(--bg);
  border-radius: 3px;
  padding: 0 0.3rem;
  font-style: italic;
}

::-webkit-scrollbar {
  width: 6px;
  height: 6px;
}

::-webkit-scrollbar-track {
  background: var(--bg);
}

::-webkit-scrollbar-thumb {
  background: var(--border);
  border-radius: 3px;
}

::-webkit-scrollbar-thumb:hover {
  background: var(--text-muted);
}
)";

static const char *SAMPLE_INDEX_MD = R"(
---
title: Home
template: page
nav: 1
---

# Welcome To My Blog

This is my personal website  
Find blogs [here](/blogs/index.html)
...
)";

static const char *SAMPLE_BLOG_INDEX_MD = R"(
---
title: Blog List
template: page
---
# Blog List

- [first blog](/blogs/first_blog.html)
- [second blog](...)
...
)";

static const char *SAMPLE_BLOG_MD = R"(
---
title: First Blog
template: blog
---

# First Blog

this is my first blog
...
)";

inline void write_file(const std::string &file, const char *content) {
  std::ofstream of(file);
  if (!of) {
    throw RuntimeError("Problem opening file: " + file,
                       "During initialization");
  }
  of << content;
}

inline void rattip_init() {
  std::filesystem::create_directory("templates");
  std::filesystem::create_directories("themes/noir");
  std::filesystem::create_directories("content/blogs");

  write_file("templates/page.html", PAGE_HTML);
  write_file("templates/blog.html", BLOG_HTML);
  write_file("themes/noir/global.css", GLOBAL_CSS);
  write_file("content/index.md", SAMPLE_INDEX_MD);
  write_file("content/blogs/index.md", SAMPLE_BLOG_INDEX_MD);
  write_file("content/blogs/first_blog.md", SAMPLE_BLOG_MD);
}

#endif  //! RATTIP_INIT_HPP

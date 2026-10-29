// book.js -- what the pages of the book do (docs/dsssl/scaly-book.dsl writes
// them): the contents in the sidebar, the theme, the search over
// searchindex.js, a copy button on every listing and Scaly coloured in it
// with the words of words.js (generated from scaly.sgm), and the keys:
// S or / searches, the arrows turn the page.
(function () {
  'use strict';
  const $ = id => document.getElementById(id);
  const root = document.documentElement;
  const body = document.body;
  const store = {
    get(key) { try { return localStorage.getItem(key); } catch (e) { return null; } },
    set(key, value) { try { if (value === null) localStorage.removeItem(key); else localStorage.setItem(key, value); } catch (e) { } },
  };
  const narrow = window.matchMedia('(max-width: 1079px)');

  // ------------------------------------------------------------ the sidebar

  const sidebar = $('sidebar');
  if (root.classList.contains('sidebar-closed-early')) {
    body.classList.add('sidebar-closed');
    root.classList.remove('sidebar-closed-early');
  }

  function set_sidebar(open) {
    if (narrow.matches) {
      body.classList.toggle('sidebar-open', open);
      body.classList.remove('sidebar-closed');
    } else {
      body.classList.toggle('sidebar-closed', !open);
      body.classList.remove('sidebar-open');
      store.set('scaly-book-sidebar', open ? null : 'closed');
    }
    $('toggle-sidebar').setAttribute('aria-expanded', String(open));
  }
  function sidebar_open() {
    return narrow.matches ? body.classList.contains('sidebar-open') : !body.classList.contains('sidebar-closed');
  }
  $('toggle-sidebar').addEventListener('click', () => set_sidebar(!sidebar_open()));
  $('scrim').addEventListener('click', () => set_sidebar(false));
  narrow.addEventListener('change', () => body.classList.remove('sidebar-open'));
  sidebar.addEventListener('click', e => {
    const fold = e.target.closest('.fold');
    if (fold) {
      const part = fold.closest('.part');
      part.classList.toggle('open');
      fold.setAttribute('aria-expanded', String(part.classList.contains('open')));
      return;
    }
    if (e.target.closest('a') && narrow.matches) set_sidebar(false);
  });

  // the sidebar keeps its place from page to page
  const scroll_key = 'scaly-book-sidebar-scroll';
  try {
    const kept = sessionStorage.getItem(scroll_key);
    if (kept !== null) sidebar.scrollTop = Number(kept);
    else sidebar.querySelector('[aria-current="page"]')?.scrollIntoView({ block: 'center' });
  } catch (e) { }
  window.addEventListener('pagehide', () => {
    try { sessionStorage.setItem(scroll_key, String(sidebar.scrollTop)); } catch (e) { }
  });

  // the section being read, marked in the sidebar
  const current = sidebar.querySelector('.part.current');
  const marks = current ? [...current.querySelectorAll('.sections a')] : [];
  const targets = marks.map(a => document.getElementById(decodeURIComponent(a.hash.slice(1)))).filter(Boolean);
  let marked_link = null;
  function mark_section() {
    let at = null;
    for (const t of targets) {
      if (t.getBoundingClientRect().top < 120) at = t; else break;
    }
    const link = at ? marks[targets.indexOf(at)] : null;
    if (link === marked_link) return;
    marked_link?.classList.remove('active');
    link?.classList.add('active');
    marked_link = link;
  }
  let pending = false;
  window.addEventListener('scroll', () => {
    if (pending) return;
    pending = true;
    requestAnimationFrame(() => { pending = false; mark_section(); });
  }, { passive: true });
  mark_section();

  // -------------------------------------------------------------- the theme

  const theme_button = $('toggle-theme');
  let menu = null;
  function close_menu() {
    menu?.remove();
    menu = null;
    theme_button.setAttribute('aria-expanded', 'false');
  }
  theme_button.addEventListener('click', e => {
    e.stopPropagation();
    if (menu) { close_menu(); return; }
    const chosen = store.get('scaly-book-theme') || 'system';
    menu = document.createElement('ul');
    menu.className = 'theme-menu';
    menu.setAttribute('role', 'menu');
    for (const [value, label] of [['system', 'As the system'], ['light', 'Light'], ['dark', 'Dark']]) {
      const li = document.createElement('li');
      const b = document.createElement('button');
      b.type = 'button';
      b.textContent = label;
      b.setAttribute('role', 'menuitemradio');
      b.setAttribute('aria-checked', String(value === chosen));
      b.addEventListener('click', () => {
        if (value === 'system') delete root.dataset.theme; else root.dataset.theme = value;
        store.set('scaly-book-theme', value === 'system' ? null : value);
        close_menu();
      });
      li.appendChild(b);
      menu.appendChild(li);
    }
    document.querySelector('.topbar').appendChild(menu);
    theme_button.setAttribute('aria-expanded', 'true');
    menu.querySelector('[aria-checked="true"]').focus();
  });
  document.addEventListener('click', e => { if (menu && !menu.contains(e.target)) close_menu(); });

  // ------------------------------------------------------- listings: Scaly

  const words = window.SCALY_WORDS || { keywords: [], operators: [], values: [] };
  const keywords = new Set(words.keywords);
  const operators = new Set(words.operators);
  const values = new Set(words.values);
  // a block comment, a line comment, a string, a quoted name, a fragment, a
  // number, a sigil before a name, a name
  const token = /(;\*[\s\S]*?\*;)|(;[^\n]*)|("(?:\\.|[^"\\\n])*")|('(?:\\.|[^'\\\n])*')|(`[^`\n]*`)|(\b0x[0-9A-Fa-f]+\b|\b\d+(?:\.\d+)?\b)|([#^!](?=[A-Za-z]))|([A-Za-z][A-Za-z0-9_]*)/g;

  function span(cls, text) {
    const s = document.createElement('span');
    s.className = cls;
    s.textContent = text;
    return s;
  }

  function highlight(code) {
    const text = code.textContent;
    const out = document.createDocumentFragment();
    let last = 0;
    for (const m of text.matchAll(token)) {
      if (m.index > last) out.appendChild(document.createTextNode(text.slice(last, m.index)));
      const [all, block, line, string, quoted, fragment, number, sigil, word] = m;
      if (block || line) out.appendChild(span('hl-comment', all));
      else if (string || fragment) out.appendChild(span('hl-string', all));
      else if (quoted) out.appendChild(document.createTextNode(all));
      else if (number) out.appendChild(span('hl-number', all));
      else if (sigil) out.appendChild(span('hl-sigil', all));
      else if (keywords.has(word)) out.appendChild(span('hl-keyword', all));
      else if (operators.has(word)) out.appendChild(span('hl-operator', all));
      else if (values.has(word)) out.appendChild(span('hl-value', all));
      else if (/^[A-Z]/.test(word)) out.appendChild(span('hl-type', all));
      else out.appendChild(document.createTextNode(all));
      last = m.index + all.length;
    }
    if (last < text.length) out.appendChild(document.createTextNode(text.slice(last)));
    code.replaceChildren(out);
  }

  // JSON, a shell session or a configuration file is left as it is
  function looks_like_scaly(text) {
    const t = text.trimStart();
    if (/^[{[]\s*"/.test(t)) return false;
    if (/^\$ /m.test(t) || /^(git|cd|curl|brew|apt|sudo|scaly|scalyc|code|export|winget|irm|iwr)\b/.test(t)) return false;
    return true;
  }

  for (const pre of document.querySelectorAll('.content pre')) {
    const code = pre.querySelector('code') || pre;
    // a listing begun on the line after its tag keeps no empty first line
    code.textContent = code.textContent.replace(/^[\r\n]+/, '').replace(/\s+$/, '');
    if (pre.classList.contains('listing') && looks_like_scaly(code.textContent)) highlight(code);
    const copy = document.createElement('button');
    copy.type = 'button';
    copy.className = 'copy';
    copy.textContent = 'Copy';
    copy.addEventListener('click', async () => {
      try {
        await navigator.clipboard.writeText(code.textContent);
        copy.textContent = 'Copied';
      } catch (e) {
        copy.textContent = 'Select and copy';
      }
      setTimeout(() => { copy.textContent = 'Copy'; }, 1500);
    });
    pre.appendChild(copy);
  }

  // ------------------------------------------------------------- the search

  const panel = $('search');
  const input = $('search-input');
  const results = $('search-results');
  const summary = $('search-summary');
  const search_button = $('toggle-search');
  const index = (window.SEARCH_INDEX || []).map(([file, anchor, number, title, text]) => ({
    file, anchor, number, title, text,
    title_lc: title.toLowerCase(), text_lc: text.toLowerCase(),
  }));
  let selected = -1;

  function open_search(open) {
    panel.hidden = !open;
    search_button.setAttribute('aria-expanded', String(open));
    if (open) { input.focus(); input.select(); }
  }
  search_button.addEventListener('click', () => open_search(panel.hidden));

  function escape(s) {
    return s.replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' })[c]);
  }
  function marked(text, terms) {
    let html = escape(text);
    for (const t of terms) {
      const re = new RegExp('(' + escape(t).replace(/[.*+?^${}()|[\]\\]/g, '\\$&') + ')', 'gi');
      html = html.replace(re, '<mark>$1</mark>');
    }
    return html;
  }
  function snippet(entry, terms) {
    const text = entry.text;
    if (!text) return '';
    let at = -1;
    for (const t of terms) {
      const i = entry.text_lc.indexOf(t);
      if (i >= 0 && (at < 0 || i < at)) at = i;
    }
    if (at < 0) return text.slice(0, 160) + (text.length > 160 ? '…' : '');
    const from = Math.max(0, text.lastIndexOf(' ', Math.max(0, at - 60)) + 1);
    const to = Math.min(text.length, from + 180);
    return (from > 0 ? '…' : '') + text.slice(from, to) + (to < text.length ? '…' : '');
  }

  function search() {
    const terms = input.value.toLowerCase().split(/\s+/).filter(t => t.length > 0);
    results.replaceChildren();
    selected = -1;
    if (terms.length === 0) { summary.textContent = ''; return; }
    const found = [];
    for (const e of index) {
      let score = 0;
      let all = true;
      for (const t of terms) {
        const in_title = e.title_lc.includes(t);
        const in_text = e.text_lc.includes(t);
        if (!in_title && !in_text) { all = false; break; }
        score += (in_title ? 10 : 0) + (in_text ? 1 : 0);
        if (new RegExp('\\b' + t.replace(/[.*+?^${}()|[\]\\]/g, '\\$&') + '\\b').test(e.title_lc)) score += 5;
      }
      if (all) found.push([score, e]);
    }
    found.sort((a, b) => b[0] - a[0]);
    summary.textContent = found.length === 0 ? 'Nothing found.'
      : found.length === 1 ? 'One section.' : found.length + ' sections' + (found.length > 40 ? ', the first 40:' : '.');
    for (const [, e] of found.slice(0, 40)) {
      const li = document.createElement('li');
      const a = document.createElement('a');
      a.href = e.file + (e.anchor ? '#' + e.anchor : '');
      a.innerHTML = '<span class="result-title"><span class="num">' + escape(e.number) + '</span>'
        + marked(e.title, terms) + '</span><span class="result-text">' + marked(snippet(e, terms), terms) + '</span>';
      a.addEventListener('click', () => open_search(false));
      li.appendChild(a);
      results.appendChild(li);
    }
  }
  input.addEventListener('input', search);
  input.addEventListener('keydown', e => {
    const links = [...results.querySelectorAll('a')];
    if (e.key === 'ArrowDown' || e.key === 'ArrowUp') {
      e.preventDefault();
      if (links.length === 0) return;
      selected = (selected + (e.key === 'ArrowDown' ? 1 : -1) + links.length) % links.length;
      links.forEach((a, i) => a.classList.toggle('selected', i === selected));
      links[selected].scrollIntoView({ block: 'nearest' });
    } else if (e.key === 'Enter' && links.length > 0) {
      links[Math.max(0, selected)].click();
    } else if (e.key === 'Escape') {
      open_search(false);
      search_button.focus();
    }
  });

  // --------------------------------------------------------------- the keys

  document.addEventListener('keydown', e => {
    if (e.ctrlKey || e.metaKey || e.altKey) return;
    const typing = /^(INPUT|TEXTAREA|SELECT)$/.test(document.activeElement?.tagName || '');
    if (typing) return;
    if (e.key === 's' || e.key === 'S' || e.key === '/') {
      e.preventDefault();
      open_search(true);
    } else if (e.key === 'Escape') {
      if (menu) close_menu();
      else if (!panel.hidden) open_search(false);
      else if (narrow.matches && sidebar_open()) set_sidebar(false);
    } else if (e.key === 'ArrowLeft') {
      const prev = document.querySelector('.pager a.prev');
      if (prev) location.href = prev.href;
    } else if (e.key === 'ArrowRight') {
      const next = document.querySelector('.pager a.next');
      if (next) location.href = next.href;
    }
  });
})();

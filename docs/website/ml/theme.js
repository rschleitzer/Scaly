// theme.js -- the reader's theme, set before the page is drawn (book.js
// changes it); without a choice the system's light or dark holds
(function () {
  try {
    var theme = localStorage.getItem('scaly-book-theme');
    if (theme === 'light' || theme === 'dark') document.documentElement.dataset.theme = theme;
    if (localStorage.getItem('scaly-book-sidebar') === 'closed' && window.matchMedia('(min-width: 1080px)').matches)
      document.documentElement.classList.add('sidebar-closed-early');
  } catch (e) { }
})();

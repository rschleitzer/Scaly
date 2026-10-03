# tests/lsp/win/sitecustomize.py -- loaded by every Python the LSP suite starts
# on the Windows box (tests/lsp/run.sh puts this directory on PYTHONPATH there
# and nowhere else). It gives the suite's ~75 Python blocks the three POSIX
# behaviours they were written against, so the blocks read the same on every
# host and the differences stand in one file:
#
#   select.select   on a PIPE. Windows' select() takes sockets only; the frame
#                   readers wait on the server's stdout. Here a descriptor is
#                   ready when PeekNamedPipe sees bytes, or the pipe is broken
#                   (the read then answers EOF at once, as on POSIX).
#   text writes     keep "\n". A fixture written in text mode would otherwise
#                   reach the disk as CRLF while the didOpen text says LF, and
#                   every offset and every saved-text comparison would differ.
#   paths           are spelled "/repos/Scaly/...": getcwd, abspath and realpath
#                   answer without the drive, join and SCALY_HOME with "/". The
#                   blocks build uris as "file://" + path, which is a well-formed
#                   uri only for a path that begins with "/", and compare what the
#                   server hands back character for character. A drive-less
#                   absolute path names the same file on the current drive, to
#                   this Python and to the server's CRT alike (isabs says so too);
#                   /tmp is C:\tmp to both, which run.sh makes the directory Git
#                   Bash calls /tmp.
#   os.symlink      falls back to a junction for a directory (no privilege).
import os

if os.name == "nt":
    import builtins, ctypes, io, msvcrt, ntpath, select, time
    from ctypes import wintypes

    _k32 = ctypes.WinDLL("kernel32", use_last_error=True)
    _peek = _k32.PeekNamedPipe
    _peek.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD,
                      ctypes.c_void_p, ctypes.POINTER(wintypes.DWORD), ctypes.c_void_p]
    _peek.restype = wintypes.BOOL

    def _pipe_ready(o):
        fd = o if isinstance(o, int) else o.fileno()
        avail = wintypes.DWORD(0)
        if not _peek(msvcrt.get_osfhandle(fd), None, 0, None, ctypes.byref(avail), None):
            return True
        return avail.value > 0

    def _select(r, w, x, timeout=None):
        deadline = None if timeout is None else time.monotonic() + timeout
        nap = 0.0005
        while True:
            ready = [o for o in r if _pipe_ready(o)]
            if ready or w or (deadline is not None and time.monotonic() >= deadline):
                return ready, list(w), []
            time.sleep(nap)
            nap = min(nap * 2, 0.01)

    select.select = _select

    _open = io.open

    def _lf_open(file, mode="r", buffering=-1, encoding=None, errors=None,
                 newline=None, closefd=True, opener=None):
        if newline is None and "b" not in mode and any(c in mode for c in "wax+"):
            newline = ""
        return _open(file, mode, buffering, encoding, errors, newline, closefd, opener)

    builtins.open = _lf_open
    io.open = _lf_open

    _drive = os.path.splitdrive(os.getcwd())[0].lower()

    def _posixish(p):
        d, rest = ntpath.splitdrive(p)
        if d.lower() == _drive:
            p = rest
        return p.replace("\\", "/")

    # A symlink needs a privilege (or Developer Mode) on Windows; a directory
    # link falls back to a JUNCTION, the reparse point every Windows user can
    # make and the one an editor's workspace behind a link would be.
    _symlink = os.symlink

    def _symlink_or_junction(src, dst, target_is_directory=False, *, dir_fd=None):
        try:
            return _symlink(src, dst, target_is_directory, dir_fd=dir_fd)
        except OSError as e:
            full = src if os.path.isabs(src) else os.path.join(os.path.dirname(dst), src)
            if getattr(e, "winerror", None) != 1314 or not os.path.isdir(full):
                raise
            import _winapi
            _winapi.CreateJunction(_abspath(full), _abspath(dst))

    os.symlink = _symlink_or_junction

    _getcwd = os.getcwd
    _abspath = os.path.abspath
    _realpath = os.path.realpath
    _join = ntpath.join
    os.getcwd = lambda: _posixish(_getcwd())
    ntpath.abspath = lambda p: _posixish(_abspath(p))
    ntpath.realpath = lambda p, **kw: _posixish(_realpath(p, **kw))
    # A join takes "/" as well: `os.path.join(tempfile.mkdtemp(), "lib.scaly")`
    # becomes a uri the server hands back character for character.
    def _slash_join(a, *p):
        r = _join(a, *p)
        return r.replace(b"\\", b"/") if isinstance(r, bytes) else r.replace("\\", "/")

    ntpath.join = _slash_join

    # `SCALY_HOME="$(pwd)" python3` arrives as "C:/repos/Scaly" (Git Bash turns
    # a POSIX path in the environment of a native program into a Windows one);
    # the server answers uris under it, the block compares them with getcwd().
    if os.environ.get("SCALY_HOME"):
        os.environ["SCALY_HOME"] = _posixish(os.environ["SCALY_HOME"])

    # Python 3.13 calls "/x" relative (to the drive); in this model it is the
    # absolute path it names on the current drive, as above.
    _isabs = ntpath.isabs
    ntpath.isabs = lambda p: _isabs(p) or str(p)[:1] in ("/", "\\")

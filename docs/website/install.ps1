# Scaly installer for Windows -- https://scaly.io
#
#   irm https://scaly.io/install.ps1 | iex
#
# Installs Scaly into %USERPROFILE%\.scaly with NO administrator rights --
# three programs:
#   scaly     the tool: `scaly run`, `scaly build`, `scaly test`; alone, the REPL
#   scalyc    the compiler, with a C compiler's command line (-o -c -S -O2)
#   scalyls   the language server the VS Code extension starts
# and beside them the standard library and the standard packages as sources.
#
# The programs are downloaded ready to run for Windows 10 and newer, x64 and
# arm64, and load nothing but what Windows brings. `scaly run`, `scaly test`
# and the REPL need nothing else. `scaly build` LINKS a program, and for that
# it needs the Visual Studio Build Tools (the "Desktop development with C++"
# workload: the linker, the C runtime's libraries, a Windows SDK) -- as Rust
# does on this target. Nothing else: no LLVM, no clang, no developer prompt;
# the compiler finds the Build Tools itself. Where they are missing this
# script says so and offers to fetch them with winget.
#
# Environment overrides:
#   SCALY_PREFIX          install location            (default %USERPROFILE%\.scaly)
#   SCALY_VERSION         version to fetch            (default 0.1.0)
#   SCALY_INSTALL_BASE    base URL for downloads, or a local directory holding
#                         the two archives            (default https://scaly.io)
#   SCALY_NO_MODIFY_PATH  set to 1 to leave PATH alone
#   SCALY_BUILD_TOOLS     ask (default) | install | skip -- what to do when the
#                         Build Tools are missing
#
# Layout:  %USERPROFILE%\.scaly\toolchain   libexec\ (the three programs, put
#                                           on PATH), lib\, packages\, seed\
#          %USERPROFILE%\.scaly\cache       compiled packages (made on first use)
# The programs find the packages beside themselves; no variable names the
# toolchain, and the directory can be moved (a SCALY_HOME that is set wins).
# Running the script again replaces the toolchain; a run that fails leaves the
# installed one as it was.
#
# Uninstall: remove %USERPROFILE%\.scaly and the PATH entry this script adds.
#
# *tests/install/run-windows.sh installs with this script from archives made
# of the tree and runs what the installed programs must be able to do. NOT
# tested there: the winget route (it installs several GB system-wide).
# Written for Windows PowerShell 5.1, which every Windows 10 has.

function Install-Scaly {
    $ErrorActionPreference = 'Stop'
    function Say([string]$m) { Write-Host "scaly-install: $m" }

    $version = if ($env:SCALY_VERSION) { $env:SCALY_VERSION } else { '0.1.0' }
    $prefix = if ($env:SCALY_PREFIX) { $env:SCALY_PREFIX } else { Join-Path $env:USERPROFILE '.scaly' }
    $base = if ($env:SCALY_INSTALL_BASE) { $env:SCALY_INSTALL_BASE } else { 'https://scaly.io' }
    $toolsMode = if ($env:SCALY_BUILD_TOOLS) { $env:SCALY_BUILD_TOOLS } else { 'ask' }

    # The machine's architecture, not this PowerShell's: an x64 PowerShell on
    # an arm64 Windows reports AMD64 for itself.
    $machine = $env:PROCESSOR_ARCHITECTURE
    if ($env:PROCESSOR_ARCHITEW6432) { $machine = $env:PROCESSOR_ARCHITEW6432 }
    # ... and neither variable says it for an emulated x64 process (nor does
    # RuntimeInformation.OSArchitecture in Windows PowerShell 5.1): the
    # machine's own value is in the registry.
    try {
        $native = (Get-ItemProperty 'HKLM:\SYSTEM\CurrentControlSet\Control\Session Manager\Environment').PROCESSOR_ARCHITECTURE
        if ($native) { $machine = $native }
    } catch { }
    if ($machine -eq 'ARM64') { $arch = 'arm64'; $vcArch = 'arm64'; $vcComponent = 'Microsoft.VisualStudio.Component.VC.Tools.ARM64' }
    elseif ($machine -eq 'AMD64') { $arch = 'x86_64'; $vcArch = 'x64'; $vcComponent = 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64' }
    else { Say "error: Scaly is handed out for x64 and arm64 Windows; this is $machine"; return }

    $tar = Join-Path $env:SystemRoot 'System32\tar.exe'
    if (-not (Test-Path $tar)) { Say "error: no $tar -- Windows 10 1803 or newer is needed"; return }

    $work = Join-Path ([System.IO.Path]::GetTempPath()) ("scaly-install-" + [System.Guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Force $work | Out-Null
    $new = Join-Path $prefix 'toolchain.new'
    $ok = $false
    $savedHome = $env:SCALY_HOME
    $savedCache = $env:SCALY_CACHE
    try {
        # ---- the two archives: the packages and the seed, the programs
        $archives = @("scaly-$version.tar.gz", "scaly-$version-windows-$arch.zip")
        foreach ($a in $archives) {
            $dest = Join-Path $work $a
            if (Test-Path $base -PathType Container) {
                $src = Join-Path $base $a
                if (-not (Test-Path $src)) { Say "error: no $src"; return }
                Copy-Item $src $dest
            } else {
                $url = "$base/downloads/$a"
                Say "fetching $url"
                [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12
                $ProgressPreference = 'SilentlyContinue'
                try { Invoke-WebRequest -UseBasicParsing -Uri $url -OutFile $dest }
                catch { Say "error: could not fetch $url ($($_.Exception.Message))"; return }
            }
        }

        New-Item -ItemType Directory -Force $prefix | Out-Null
        if (Test-Path $new) { Remove-Item -Recurse -Force $new }
        New-Item -ItemType Directory -Force $new | Out-Null
        foreach ($a in $archives) {
            & $tar -x -f (Join-Path $work $a) -C $new | Out-Host
            if ($LASTEXITCODE -ne 0) { Say "error: $a did not unpack"; return }
        }
        $libexec = Join-Path $new 'libexec'
        foreach ($p in 'scalyc', 'scaly', 'scalyls') {
            if (-not (Test-Path (Join-Path $libexec "$p.exe"))) { Say "error: the archive holds no $p.exe"; return }
        }

        # ---- the programs run here: a program through the in-process JIT
        # no SCALY_HOME: the programs are to find the toolchain they lie in
        $env:SCALY_HOME = $null
        $env:SCALY_CACHE = Join-Path $work 'cache'
        $hello = Join-Path $work 'hello.scaly'
        [System.IO.File]::WriteAllText($hello, "print(`"Hello from Scaly!`")`n")
        $ErrorActionPreference = 'Continue'
        $out = & (Join-Path $libexec 'scaly.exe') run $hello
        $rc = $LASTEXITCODE
        $ErrorActionPreference = 'Stop'
        if ($rc -ne 0 -or "$out".Trim() -ne 'Hello from Scaly!') {
            Say "error: the installed tool does not run a program (rc $rc, '$out')"
            return
        }
        Say "scaly run: ok"

        # ---- the Build Tools: what `scaly build` links with
        $pf86 = ${env:ProgramFiles(x86)}
        $haveTools = {
            $vswhere = Join-Path $pf86 'Microsoft Visual Studio\Installer\vswhere.exe'
            if (-not (Test-Path $vswhere)) { return }
            $found = $false
            foreach ($vs in @(& $vswhere -utf8 -products * -property installationPath)) {
                if ($vs -and (Test-Path (Join-Path $vs "VC\Tools\MSVC\*\lib\$vcArch\libcmt.lib"))) { $found = $true }
            }
            $sdkRoot = if ($env:WindowsSdkDir) { $env:WindowsSdkDir } else { Join-Path $pf86 'Windows Kits\10' }
            $found -and (Test-Path (Join-Path $sdkRoot "Lib\*\um\$vcArch\kernel32.lib"))
        }
        $tools = & $haveTools
        if (-not $tools -and $toolsMode -ne 'skip') {
            Say "the Visual Studio Build Tools (C++ tools for $vcArch and a Windows SDK) are not installed."
            Say "scaly run, scaly test and the REPL work without them; scaly build needs them to link."
            $winget = Get-Command winget -ErrorAction SilentlyContinue
            $override = "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended --add $vcComponent"
            $line = "winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override `"$override`""
            $fetch = $false
            if ($toolsMode -eq 'install') { $fetch = $true }
            elseif ($winget -and [Environment]::UserInteractive -and -not [Console]::IsInputRedirected) {
                $answer = Read-Host "scaly-install: fetch them now with winget (several GB, asks for administrator rights)? [y/N]"
                $fetch = $answer -match '^[yYjJ]'
            }
            if ($fetch -and $winget) {
                Say "running: $line"
                $ErrorActionPreference = 'Continue'
                & winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override $override | Out-Host
                $ErrorActionPreference = 'Stop'
                $tools = & $haveTools
                if (-not $tools) { Say "the Build Tools are still not found; scaly build will say so when it is asked to link" }
            } else {
                Say "to install them later:  $line"
                Say "(or the Visual Studio Installer: workload `"Desktop development with C++`")"
            }
        }
        if ($tools) {
            $exe = Join-Path $work 'hello.exe'
            $ErrorActionPreference = 'Continue'
            & (Join-Path $libexec 'scaly.exe') build $hello -o $exe | Out-Host
            $rc = $LASTEXITCODE
            $out = if ($rc -eq 0) { & $exe } else { '' }
            $ErrorActionPreference = 'Stop'
            if ($rc -ne 0 -or "$out".Trim() -ne 'Hello from Scaly!') {
                Say "error: the installed tool does not build a program (rc $rc, '$out')"
                return
            }
            Say "scaly build: ok"
        }

        # ---- into place: only now is the installed toolchain touched
        $toolchain = Join-Path $prefix 'toolchain'
        $old = Join-Path $prefix 'toolchain.old'
        if (Test-Path $old) { Remove-Item -Recurse -Force $old }
        if (Test-Path $toolchain) { Rename-Item $toolchain 'toolchain.old' }
        Rename-Item $new 'toolchain'
        if (Test-Path $old) { Remove-Item -Recurse -Force $old }
        $bin = Join-Path $toolchain 'libexec'

        if ($env:SCALY_NO_MODIFY_PATH -eq '1') {
            Say "installed into $toolchain"
            Say "PATH left alone: put $bin on it"
        } else {
            $userPath = [Environment]::GetEnvironmentVariable('Path', 'User')
            if (-not $userPath) { $userPath = '' }
            if (($userPath -split ';') -notcontains $bin) {
                [Environment]::SetEnvironmentVariable('Path', (($userPath.TrimEnd(';') + ';' + $bin).TrimStart(';')), 'User')
            }
            # this session too
            if (($env:Path -split ';') -notcontains $bin) { $env:Path = "$env:Path;$bin" }
            Say "installed into $toolchain; scaly, scalyc and scalyls are on PATH (new terminals see it)"
        }
        Say "try:  scaly            (the REPL)"
        Say "      scaly run hello.scaly"
        $ok = $true
        $script:scalyInstalled = $true
    } finally {
        $env:SCALY_HOME = $savedHome
        $env:SCALY_CACHE = $savedCache
        if (-not $ok -and (Test-Path $new)) { Remove-Item -Recurse -Force $new -ErrorAction SilentlyContinue }
        Remove-Item -Recurse -Force $work -ErrorAction SilentlyContinue
    }
}

$script:scalyInstalled = $false
Install-Scaly
# As a file (powershell -File install.ps1) the exit code says how it went;
# piped into iex there is no file, and `exit` would close the user's terminal.
if ($MyInvocation.MyCommand.Path) {
    if ($script:scalyInstalled) { exit 0 } else { exit 1 }
}

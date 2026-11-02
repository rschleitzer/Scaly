#!/bin/sh
# Builds scaly-visualstudio.vsix with Visual Studio's MSBuild (`dotnet build`
# does not find the VSSDK targets). Needs Visual Studio 2026 with the workload
# "Visual Studio extension development".
#
#   ./build.sh [Debug|Release]          the VSIX in Scaly.VisualStudio/bin/<config>/net472/
#   ./build.sh --exp [Debug|Release]    also deploy it into the Experimental instance
#                                       (start it with: devenv /rootSuffix Exp <folder>;
#                                       close that instance first; its
#                                       configuration is rebuilt after the deploy)
set -eu

deploy=false
if [ "${1:-}" = "--exp" ]; then
    deploy=true
    shift
fi
config=${1:-Debug}

here=$(cd "$(dirname "$0")" && pwd)
vswhere="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
root=$("$vswhere" -latest -prerelease -requires Microsoft.VisualStudio.Workload.VisualStudioExtension -property installationPath | tr -d '\r')
if [ -z "$root" ]; then
    echo "build.sh: no Visual Studio with the workload 'Visual Studio extension development'" >&2
    exit 1
fi
root=$(cygpath -u "$root")

# The MSBuild of the machine's own architecture.
msbuild=
for arch in arm64 amd64 .; do
    if [ -x "$root/MSBuild/Current/Bin/$arch/MSBuild.exe" ]; then
        msbuild="$root/MSBuild/Current/Bin/$arch/MSBuild.exe"
        break
    fi
done

set -- -restore -nologo -verbosity:minimal "-p:Configuration=$config"
if $deploy; then
    set -- "$@" -p:DeployExtension=true -p:VSSDKTargetPlatformRegRootSuffix=Exp
fi

# Git Bash would rewrite the /-switches into paths.
MSYS_NO_PATHCONV=1 "$msbuild" "$@" "$(cygpath -w "$here/Scaly.VisualStudio/Scaly.VisualStudio.csproj")"

# A deployed extension is not always seen at the next start: an extension
# folder that went away (or came) without the installer leaves the instance's
# extension cache stale. Rebuild it, as the installer would.
if $deploy; then
    MSYS_NO_PATHCONV=1 "$root/Common7/IDE/devenv.com" /rootSuffix Exp /updateconfiguration
fi

echo "$here/Scaly.VisualStudio/bin/$config/net472/scaly-visualstudio.vsix"

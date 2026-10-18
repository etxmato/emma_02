# Provision VS 2022 Build Tools for the Emma 02 Windows builder (guest-side).
# Installs the Desktop C++ workload + ARM64/x86/x64 toolsets + MSBuild + a Win11 SDK.
$ErrorActionPreference = 'Continue'
$vsArgs = @(
  '--quiet','--wait','--norestart','--nocache',
  '--add','Microsoft.VisualStudio.Workload.VCTools','--includeRecommended',
  '--add','Microsoft.VisualStudio.Component.VC.Tools.ARM64',
  '--add','Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
  '--add','Microsoft.Component.MSBuild',
  '--locale','en-US'
)
Write-Output "starting VS Build Tools install..."
$p = Start-Process -FilePath 'C:\Users\etxmato\vs_BuildTools.exe' -ArgumentList $vsArgs -Wait -PassThru
Write-Output ("VSBT_RC=" + $p.ExitCode)

# Report resulting MSBuild location
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
  $msb = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' 2>$null
  Write-Output ("MSBUILD=" + $msb)
} else {
  Write-Output "VSWHERE-MISSING"
}

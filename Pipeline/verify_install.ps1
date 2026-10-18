# Verify one Emma 02 Windows installer in the guest: uninstall any previous,
# silent-install, report PE type + runtime dlls, metadata-junk counts and the
# presence of the needed leading-dot data files, then uninstall.
param([Parameter(Mandatory=$true)][string]$Installer)
$ErrorActionPreference = 'Continue'

$dirs = @('C:\Program Files\Emma 02','C:\Program Files (x86)\Emma 02')
foreach ($d in $dirs) {
  $u = Join-Path $d 'unins000.exe'
  if (Test-Path $u) { Start-Process $u -ArgumentList '/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART' -Wait; Start-Sleep 2 }
}

Write-Output ("=== installing " + $Installer + " ===")
$proc = Start-Process -FilePath $Installer -ArgumentList '/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART','/SP-' -Wait -PassThru
Write-Output ("INSTALL_RC=" + $proc.ExitCode)

$AppDir = ''
foreach ($d in $dirs) { if (Test-Path (Join-Path $d 'Emma 02.exe')) { $AppDir = $d; break } }
if ($AppDir -eq '') { Write-Output 'EXE-MISSING'; Write-Output 'VERIFY_DONE'; exit 1 }
Write-Output ("APPDIR=" + $AppDir)

$exe = Join-Path $AppDir 'Emma 02.exe'
$a = [IO.File]::ReadAllBytes($exe)
$pe = $a[0x3C] + ($a[0x3D] * 256)
$m  = $a[$pe+4] + ($a[$pe+5] * 256)
$mt = switch ($m) { 0x8664 {'x64'} 0x14c {'x86'} 0xaa64 {'ARM64'} default {('0x' + $m.ToString('X'))} }
Write-Output ("PE=" + $mt)
Write-Output ("EXE_BYTES=" + $a.Length)
Write-Output "--- runtime dlls ---"
Get-ChildItem $AppDir -Filter '*.dll' | ForEach-Object { Write-Output $_.Name }

$all = Get-ChildItem $AppDir -Recurse -Force -ErrorAction SilentlyContinue
Write-Output ("._* files       : " + @($all | Where-Object { $_.Name -like '._*' }).Count + "   (want 0)")
Write-Output (".DS_Store files : " + @($all | Where-Object { $_.Name -eq '.DS_Store' }).Count + "   (want 0)")

Write-Output '--- needed leading-dot data files present? ---'
foreach ($rel in @('data\EDUKIT\.dummy.txt','data\Microboard\BASIC\.BASIC1.CMP','data\MS2000\BASIC\.BASIC1.CMP','data\MS2000\MICRODOS\.DIR.CM')) {
  $f = Join-Path $AppDir $rel
  if (Test-Path -LiteralPath $f) { Write-Output ("PRESENT  " + $rel) } else { Write-Output ("ABSENT   " + $rel) }
}

foreach ($d in $dirs) {
  $u = Join-Path $d 'unins000.exe'
  if (Test-Path $u) { Start-Process $u -ArgumentList '/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART' -Wait }
}
Write-Output 'UNINSTALLED'
Write-Output 'VERIFY_DONE'

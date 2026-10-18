# Remove macOS metadata files that survive in the guest build tree between runs.
# tar extraction never deletes files that are absent from the archive, so stale
# AppleDouble (._*) and .DS_Store files from an earlier tree copy would otherwise
# persist in C:\emma02-build and be packaged into the installers.
$root = 'C:\emma02-build'
$junk = Get-ChildItem -Path $root -Recurse -Force -ErrorAction SilentlyContinue |
  Where-Object {
    $_.Name -like '._*' -or
    $_.Name -eq '.DS_Store' -or
    $_.Name -eq '.AppleDouble' -or
    $_.Name -eq '.LSOverride'
  }
# NOTE: the pre-rename "Linux build" directory is removed by the pipeline via
# cmd (rmdir /s /q) before this script runs - kept out of here because
# Remove-Item on that path proved unreliable through the ssh/cmd/powershell chain.
$n = @($junk).Count
if ($n -gt 0) { $junk | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue }
Write-Output ("PURGE_DONE removed=" + $n)

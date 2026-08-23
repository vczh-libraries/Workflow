param(
    [Parameter(Position = 0)]
    [string]$SkippedTestCaseListFile
)

$ErrorActionPreference = 'Stop'

$bin = Join-Path $PSScriptRoot 'UnitTest\x64\Debug'
$driver = Join-Path $bin 'RpcStdioTest_Driver.exe'
$service = Join-Path $bin 'RpcStdioTest_Service.exe'

if (-not (Test-Path -LiteralPath $driver -PathType Leaf)) {
    throw "RpcStdioTest_Driver is not built in Debug x64: $driver"
}
if (-not (Test-Path -LiteralPath $service -PathType Leaf)) {
    throw "RpcStdioTest_Service is not built in Debug x64: $service"
}

$driverArguments = @('"{0}"' -f $service)
if ($SkippedTestCaseListFile) {
    $skipFile = (Resolve-Path -LiteralPath $SkippedTestCaseListFile -ErrorAction Stop).Path
    $driverArguments += $skipFile
}

Write-Host "Starting: $driver $($driverArguments -join ' ')"
& $driver @driverArguments
exit $LASTEXITCODE

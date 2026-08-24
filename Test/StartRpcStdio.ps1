param(
    [Parameter(Position = 0)]
    [string]$SkippedTestCaseListFile,

    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',

    [ValidateSet('Win32', 'x64')]
    [string]$Platform = 'x64'
)

$ErrorActionPreference = 'Stop'

$bin = Join-Path $PSScriptRoot 'UnitTest'
if ($Platform -eq 'x64') {
    $bin = Join-Path $bin 'x64'
}
$bin = Join-Path $bin $Configuration
$driver = Join-Path $bin 'RpcStdioTest_Driver.exe'
$service = Join-Path $bin 'RpcStdioTest_Service.exe'

if (-not (Test-Path -LiteralPath $driver -PathType Leaf)) {
    throw "RpcStdioTest_Driver is not built in ${Configuration} ${Platform}: $driver"
}
if (-not (Test-Path -LiteralPath $service -PathType Leaf)) {
    throw "RpcStdioTest_Service is not built in ${Configuration} ${Platform}: $service"
}

$driverArguments = @('"{0}"' -f $service)
if ($SkippedTestCaseListFile) {
    $driverArguments += (Resolve-Path -LiteralPath $SkippedTestCaseListFile -ErrorAction Stop).Path
}

Write-Host "Starting: $driver $($driverArguments -join ' ')"
& $driver @driverArguments
exit $LASTEXITCODE

$Script:TcfBin = "tcf"
if (Get-Command "tcf" -ErrorAction SilentlyContinue) {
    $Script:TcfBin = "tcf"
} elseif (Test-Path "$PSScriptRoot/../../target/debug/tcf-rs.exe") {
    $Script:TcfBin = Resolve-Path "$PSScriptRoot/../../target/debug/tcf-rs.exe" | Select-Object -ExpandProperty Path
} elseif (Test-Path "$PSScriptRoot/../../target/debug/tcf-rs") {
    $Script:TcfBin = Resolve-Path "$PSScriptRoot/../../target/debug/tcf-rs" | Select-Object -ExpandProperty Path
}

function Get-TcfValue {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory=$true, Position=0)]
        [string]$Key,
        
        [Parameter(Position=1)]
        [string]$File = "build.tcf"
    )
    
    $result = & $Script:TcfBin --file $File get $Key 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to get key: $Key. Error: $result"
        return $null
    }
    return $result
}

function Invoke-TcfTask {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory=$true, Position=0)]
        [string]$Task,
        
        [Parameter(Position=1)]
        [string]$File = "build.tcf"
    )
    
    & $Script:TcfBin --file $File run $Task
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Task $Task failed."
    }
}

function Get-TcfTasks {
    [CmdletBinding()]
    param (
        [Parameter(Position=0)]
        [string]$File = "build.tcf"
    )
    
    & $Script:TcfBin --file $File list
}

Export-ModuleMember -Function Get-TcfValue, Invoke-TcfTask, Get-TcfTasks

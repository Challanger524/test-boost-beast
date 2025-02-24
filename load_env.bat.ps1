# Load Visual Studio environment variables from `vcvars.bat` with PowerShell
# Script originates from:
# PowerShell - fetching env variables after running a bat/cmd file [duplicate]: https://stackoverflow.com/a/77112783/11753532
# Example usage:
# powershell -File .\vcvars.ps1 -f "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
param (
  [string] $f = "nofile"
)

#Write-Output "$f"

# Get the current list of env. vars. and their values, in the same
# format that cmd.exe's SET command produces.
$envVarsBefore = Get-ChildItem Env: | ForEach-Object { '{0}={1}' -f $_.Name, $_.Value }
$envVarsBefore | Out-File _envVarsBefore.txt # save to file

# Synchronously execute the batch file and call SET from the same cmd.exe
# session to list all process-level env. vars.
# NOTE:
#  * For simplicity, the batch file's own output is *discarded* (>NUL)
#    See the notes below this code if you want to pass it through.
#  * "PROMPT" is excluded, as it is only relevant to cmd.exe

#$envVarsAfter = cmd /C '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >NUL & SET' | Where-Object { $_ -notmatch '^PROMPT=' }
$envVarsAfter = cmd /C """$f"" >NUL & SET" | Where-Object { $_ -notmatch '^PROMPT=' }
$envVarsAfter | Out-File _envVarsAfter.txt # save to file

# Now compare the two lists and define the variables in PowerShell
# that were added or updated by the batch file, and undefine (remove)
# those that were removed.
Compare-Object -PassThru $envVarsBefore $envVarsAfter |
  ForEach-Object {
    if ($_.SideIndicator -eq '=>') {   # Variable was added or updated
      $name, $value = $_ -split '=', 2 # Split the "<name>=<value>" line into the variable's name and value.
      $output = $_.SideIndicator + $name; Write-Output $output # print additions and changed
      Set-Content Env:$name $value     # Define it as a process-level environment variable in PowerShell.
    }
    else { # $_.SideIndicator -eq '<=' # Variable was removed # false triggers on changed `PATH` variable
      $name = ($_ -split '=', 2)[0]    # Remove the variable
      $output = $_.SideIndicator + $name;
      $changed = $envVarsBefore | Select-String -Pattern $name -CaseSensitive -SimpleMatch

      if ($changed -eq "") { # detect changes as deletions
        $output += " - deleted"
        #Remove-Item Env:$name
      }
      else {
        $output += " - changed"
      }

      Write-Output $output # print deleted and changed
    }
  }

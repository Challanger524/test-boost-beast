# Template C++

### Branch Feature
#### Empty `boost::` with necessary submodules `init`

```sh
git clone --branch init_subms "https://github.com/Challanger524/test-boost-beast"
```
```sh
misc/script/init_subms # .sh/.ps1
```

```sh
#git clone --depth 1 "https://github.com/boostorg/boost"


git submodule update --init --depth 1 -- [other_than_beast_subms] # from .gitmodules
git submodule update --init --depth 1 -- [libs/beast direct_deps] # from CMakeLists.txt
.\dist\bin\boostdep beast # direct_deps from CMakeLists.txt
<boost/[^/.]*\.hpp> # module-less headers
<boost/[^/]*        # capture module

git submodule update --init --depth 1 --jobs=4 -- [primaries] # from first boostdeb run
...
git submodule update --init --depth 1 --jobs=4 -- [secondaries] # from second boostdeb run
... # how much/deep?
```

### Prerequisites

- boost [repo](https://github.com/boostorg/boost) in parent folder (`../boost`)
  ```sh
  git clone --depth 1 --recurse --shallow-submodules --jobs=4 "https://github.com/boostorg/boost"
  ```

### Features
- `Clang-CL` compiler cmake preset (non Visual Studio)
- Load Visual Studio environment in `CMake`
- Load Visual Studio environment in `PowerShell`
- Visual Studio environment loading alternatives:
  - use env from vcvars.bat: `vcvars64.bat` or `vcvarsall.bat x64`
  - VS Code: `"cmake.useVsDeveloperEnvironment": "always"`
    - `"auto"` stated to be able to detect `clang-cl` what is not happening

# Template C++

```sh
git clone --recurse "https://github.com/Challanger524/test-boost-beast"
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

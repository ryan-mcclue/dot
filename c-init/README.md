<!-- SPDX-License-Identifier: zlib-acknowledgement -->
# Desktop 
![Desktop Lint and Test](https://github.com/ryan-mcclue/desktop/actions/workflows/desktop.yml/badge.svg)
Desktop application

## Linux
```
# Optional if want release build
mkdir private
echo 'param_mode="release"' > private/build-params

bash misc/build "tests"
./build/desktop-tests-debug

bash misc/build "app"
./build/desktop-debug
```

<!-- SPDX-License-Identifier: zlib-acknowledgement -->
# Desktop 
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

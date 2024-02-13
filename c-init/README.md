<!-- SPDX-License-Identifier: zlib-acknowledgement -->
# Desktop 
> ![Workflow](https://github.com/ryan-mcclue/desktop/actions/workflows/desktop-actions.yml/badge.svg)

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

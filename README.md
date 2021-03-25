# QtLottie

QtLottie is a tiny C++ library that can render [Adobe® After Effects™](http://www.adobe.com/products/aftereffects.html) animations exported as JSON with [Bodymovin](https://github.com/airbnb/lottie-web).

## Features

- Supported many After Effects features. [Click here to see a complete list](https://github.com/Samsung/rlottie#supported-after-effects-features).
- Cross-platform: support Windows (7~10), Linux and macOS.
- Support both Qt Widgets and Qt Quick.
- Support all versions from Qt5 to Qt6.
- Very easy to build and use.

## Build

```bash
cmake .
cmake --build .
cmake --install .
```

[rlottie](https://github.com/Samsung/rlottie) is the only third-party dependency. QtLottie will load it dynamically at run-time. Make sure you have build it and put it in the executable's directory before you run your application.

## Usage

See [examples](/examples).

## Limitation

If your JSON uses assets, these files can only be placed on the disk, they can't be compressed or embeded into any container-like thing. It's caused by technical limitation, PRs are  welcome.

## License

```text
MIT License

Copyright (C) 2021 wangwenx190 (Yuhang Zhao)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

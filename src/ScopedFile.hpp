/*
MIT License

Copyright (c) 2025 Cellie https://github.com/CelliesProjects/LGFX-ScreenShot

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
*/
#ifndef SCOPEDFILE_HPP
#define SCOPEDFILE_HPP

#include <Arduino.h>
#include <FS.h>

class ScopedFile
{
public:
    ScopedFile(const char *filename, FS &fs)
        : file_(fs.open(filename, FILE_WRITE)) {}

    ScopedFile(const String &filename, FS &fs)
        : file_(fs.open(filename.c_str(), FILE_WRITE)) {}

    File &get() { return file_; }
    bool isValid() const { return file_; }

    // Rule of 5: prevent copying, but moves are fine
    ScopedFile(const ScopedFile &) = delete;
    ScopedFile &operator=(const ScopedFile &) = delete;
    ScopedFile(ScopedFile &&) = default;
    ScopedFile &operator=(ScopedFile &&) = default;

private:
    File file_;
};

#endif // SCOPEDFILE_H

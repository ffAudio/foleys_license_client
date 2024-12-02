License
=======

This software is provided under the ISC License

Copyright 2024 - Daniel Walz 
- Foleys Finest Audio UG (haftungsbeschraenkt)
- Lobith Audio UG (haftungsbeschraenkt)

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted,
provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.


Dependencies
------------

- Base64 encoding/decoding:    
  The base64 code is taken from Julian Storer's CHOC library (ISC license):    
  https://github.com/Tracktion/choc

- JSON encoding/decoding:    
  JSON is processed using Niels Lohmann's json library (MIT license)    
  https://github.com/nlohmann/json

- Cryptography:    
  The communication is secured using libsodium (ISC license):    
  https://github.com/jedisct1/libsodium

- File handling:  
  Gulrak filesystem is a drop in replacement for std::filesystem  (MIT license)  
  https://github.com/gulrak/filesystem

- Folder lookup:  
  sago007 platform_folders for finding the application data (MIT license)  
  https://github.com/sago007/PlatformFolders

- Optional JUCE:    
  This bundle provides examples for JUCE which is neither included nor required to use    
  JUCE is available under a commercial license and must be acquired separately from    
  https://juce.com/get-juce


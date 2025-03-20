# Ubaldo's define
A (*nix) terminal tool for getting a words definition written in C. This tool uses the cJSON library and [Free Dictionary API](https://github.com/meetDeveloper/freeDictionaryAPI)

## Usage
Run `$ define <word>`

## Dependencies
Uses the cJSON library, be sure to install before running/compiling

## Compiling 
```
git clone "https://github.com/cucarrillo/define.git"
cd define
make
```

## Installation
```
make install
```

## Uninstallation
```
make uninstall
```

## Example
```
$ define define
[noun]
A kind of macro in source code that replaces one text string with another wherever it occurs.

[verb]
To determine with precision; to mark out with distinctness; to ascertain or exhibit clearly.

[source]
https://en.wiktionary.org/wiki/define
```

## Comming Soon
- Options such as no color, no link, simple view
- Offline version

# FLPatch
Config-based patching plugin for Freelancer and FLServer which serves as a proof of concept replacement for most manual hex edits. Please keep in mind that this plugin performs patches dynamically. As a result, some patches that are applied this way may have no effect due to memory being read *before* the plugin gets loaded. In such cases, the patch will have to be applied using a hex edit instead. See the wiki page for patches that cannot be applied with FLPatch: https://github.com/BC46/FLPatch/wiki.

## Installation instructions
Download the latest `FLPatch.dll` from [Releases](https://github.com/BC46/FLPatch/releases). Extract the `dll` to the `EXE` folder of your Freelancer installation.
Next, open `dacom.ini` and `dacomsrv.ini`, and for both files add `FLPatch.dll` to the `[Libraries]` section.

## Usage instructions
In the `EXE` folder of your Freelancer installation, create a new file called `patches.ini`. FLPatch will apply all patches specified in this file while either Freelancer or FLServer is running. Example patch:

```ini
[Patch]
module = common.dll
offset = 1234AB
type = f
value = 42
```

This will patch file offset `1234AB` in `common.dll` and set the float value `42` at that location.

### Patch parameters documentation
| Name     | Required / optional | Data type | Description |
| -------- | --------- | --------- | --------- |
| module   | Required  | String               | Name of the binary file in which the patch should be applied. E.g. `common.dll`, `freelancer.exe`. For `.dll` files, the extension may be omitted, so e.g. `common` is also fine. |
| offset   | Required  | String in hex-format | The file offset, i.e. the patch location within the given `module`, in hex-format. Example: `1234AB`. |
| type     | Optional  | String               | FLPatch supports the following value types: Hexadecimal, 32-bit signed integer, signed byte, float, double. It suffices to specify these value types using `h`, `i`, `b`, `f`, `d`, respectively. Though it is also fine to e.g. specify signed byte by entering `byte`, or `BytE` (with upper case letters) as long as the string starts with one of the previously mentioned characters. The default value for type is Hexadecimal. |
| value    | Required  | Depends on `type`    | Depending on the chosen `type`, a value of the corresponding type will be expected here.<br /><br />For Hexadecimal, a string should be given in hexadecimal format, e.g. `EB 07 12`, `c9aee0`. Spaces between the bytes are optional and both upper case and lower case-letters are recognized.<br /><br />For a 32-bit signed integer, an integer should be given which fits in the supported range.<br /><br />For signed bytes, values should be given between -128 and 127 (inclusive).<br /><br />For floats, a float value is expected (4 bytes). <br /><br />**NOTE:** Double values are retrieved as floats (4 bytes) and later converted to doubles (8 bytes). Hence more elaborate double values may lose their precision. |

**NOTE**: The order in which the parameters `module`, `offset`, and `type` are specified does not matter, but `value` must always come last.

## Build instructions
Build the project using the `NMAKE` tool from Microsoft Visual C++ 6.0 (VC6 or Visual Studio 98) with the included makefile.
This will output the `FLPatch.dll` file in the `bin` folder.
Building the project using other compilers may require the code to be modified.
